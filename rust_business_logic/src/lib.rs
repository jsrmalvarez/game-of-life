extern crate cty;
use std::sync::{Mutex, MutexGuard, OnceLock};
use cty::{c_int, size_t};

#[derive(Debug, Copy, Clone, PartialEq)]
enum CellState {
    Alive,
    Dead,
    AliveCalculating(usize),
    DeadCalculating(usize),
}

struct Board {
    cells: Vec<Vec<CellState>>,
    width: usize,
    height: usize,
}

static BOARD_STATE:OnceLock<Mutex<Board>> = OnceLock::new();

impl Board {

    fn instance() -> MutexGuard<'static, Board>{
        BOARD_STATE.get().expect("Cannot get board").lock().unwrap_or_else(|_| panic!("Cannot unlock board"))
    }

    fn initialize(width: usize, height: usize){
        let cells = vec![vec![CellState::Dead; width]; height];
        let board = Board { cells, width, height };
        let mutex: Mutex<Board> = Mutex::new(board);
        BOARD_STATE.set(mutex).unwrap_or_else(|_| panic!("Board already initialized"));
    }
}

fn rust_init(width: usize, height: usize) {
    Board::initialize(width, height);
    print_number_of_live_cells();

}

fn print_number_of_live_cells(){
    let mut number_live_cells = 0;

    let width = Board::instance().width;
    let height = Board::instance().height;
    for x in 0..width {
        for y in 0..height {
            let current_cell = rust_get_cell(x, y);
            match current_cell {
                CellState::Alive => number_live_cells +=1,
                _ => (),
            }
        }
    }

    println!("Remaining live cells: {}", number_live_cells);
}

fn rust_get_cell(x: usize, y: usize) -> CellState {
    Board::instance().cells[x][y]
}

fn rust_set_cell(x: usize, y: usize, state: CellState) {
    Board::instance().cells[x][y] = state;
}

fn rust_get_alive_neighbohrs(x: usize, y: usize) -> usize {
    let x_minus_1 = x.checked_sub(1).unwrap_or_else(|| Board::instance().width - 1);
    let y_minus_1: usize = y.checked_sub(1).unwrap_or_else(|| Board::instance().height - 1);
    let x_plus_1 = (x + 1) % Board::instance().width;
    let y_plus_1 = (y + 1) % Board::instance().height;

    let neighbors: [CellState;8]  = [
        rust_get_cell(x_minus_1, y_minus_1),
        rust_get_cell(x_minus_1, y),
        rust_get_cell(x_minus_1, y_plus_1),
        rust_get_cell(x, y_minus_1),
        rust_get_cell(x, y_plus_1),
        rust_get_cell(x_plus_1, y_minus_1),
        rust_get_cell(x_plus_1, y),
        rust_get_cell(x_plus_1, y_plus_1),
    ];

    neighbors.iter().filter(|&cell| {
        match *cell{
            CellState::Alive | CellState::AliveCalculating(_) => true,
            _ => false}
        }).count()
}

fn rust_tick() {
    // Calculate number of alive neighbors for each cell
    let width = Board::instance().width;
    let height = Board::instance().height;
    for x in 0..width {
        for y in 0..height {
            let num_alive_neighbohrs = rust_get_alive_neighbohrs(x, y);
            let current_cell = rust_get_cell(x, y);
            match current_cell {
                CellState::Alive => rust_set_cell(x, y, CellState::AliveCalculating(num_alive_neighbohrs)),
                CellState::Dead => rust_set_cell(x, y, CellState::DeadCalculating(num_alive_neighbohrs)),
                _ => panic!("Cannot calculate alive neighbors for an already calculating cell"),
            }
        }
    }

    let mut number_live_cells = 0;

    // Apply Game of Life rules
    for x in 0..width {
        for y in 0..height {
            let current_cell = rust_get_cell(x, y);
            match current_cell {
                CellState::AliveCalculating(num_alive_neighbohrs) if num_alive_neighbohrs < 2 =>
                        rust_set_cell(x, y, CellState::Dead), // Underpopulation
                CellState::AliveCalculating(num_alive_neighbohrs) if num_alive_neighbohrs > 3 =>
                        rust_set_cell(x, y, CellState::Dead), // Overpopulation
                CellState::AliveCalculating(_) =>
                    {
                        rust_set_cell(x, y, CellState::Alive);  // Lives on
                        number_live_cells += 1;
                    },
                CellState::DeadCalculating(num_alive_neighbohrs) if num_alive_neighbohrs == 3 =>
                    {
                        rust_set_cell(x, y, CellState::Alive); // Reproduction
                        number_live_cells += 1;
                    },
                CellState::DeadCalculating(_) =>
                        rust_set_cell(x, y, CellState::Dead), // Stays dead
                _ => panic!("Cannot apply Game of Life rules to a cell that is not calculating"),
            }

        }
    }

    print_number_of_live_cells();

}


#[no_mangle]
pub extern "C" fn init(width:size_t, height:size_t){
    rust_init(width, height);
}

#[no_mangle]
pub extern "C" fn set_cell(x:c_int, y:c_int, alive:bool){
    rust_set_cell(x.try_into().unwrap(), y.try_into().unwrap(), if alive { CellState::Alive } else { CellState::Dead });
}

#[no_mangle]
pub extern "C" fn get_cell(x:c_int, y:c_int) -> bool {
    rust_get_cell(x.try_into().unwrap(), y.try_into().unwrap()) == CellState::Alive
}

#[no_mangle]
pub extern "C" fn tick(){
    rust_tick();
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    #[should_panic(expected = "Board already initialized")]
    fn cannot_reinitialize() {
        rust_init(10, 10);
        rust_init(10, 10);
    }

    #[test]
    fn can_tell_alive_neighbohrs() {
        let _ =std::panic::catch_unwind(|| {
            rust_init(10, 10);
        });
        rust_set_cell(0, 0, CellState::Alive);
        rust_set_cell(0, 1, CellState::Alive);
        rust_set_cell(1, 0, CellState::Alive);
        rust_set_cell(1, 1, CellState::Alive);
        rust_set_cell(9, 0, CellState::Alive);
        rust_set_cell(9, 1, CellState::Alive);
        rust_set_cell(0, 9, CellState::Alive);
        rust_set_cell(1, 9, CellState::Alive);
        rust_set_cell(9, 9, CellState::Alive);
        assert_eq!(rust_get_alive_neighbohrs(0, 0), 8);
        assert_eq!(rust_get_alive_neighbohrs(0, 1), 5);
        assert_eq!(rust_get_alive_neighbohrs(1, 0), 5);
        assert_eq!(rust_get_alive_neighbohrs(1, 1), 3);

    }

    #[test]
    fn alive_cell_will_live_on_with_2_neighbohrs() {
        let _ =std::panic::catch_unwind(|| {
            rust_init(10, 10);
        });
        rust_set_cell(0, 0, CellState::Alive);
        rust_set_cell(0, 1, CellState::Alive);
        rust_set_cell(1, 0, CellState::Alive);

        assert_eq!(rust_get_alive_neighbohrs(0, 0), 2);
        rust_tick();
        assert_eq!(rust_get_cell(0, 0), CellState::Alive);
        assert_eq!(rust_get_cell(1, 0), CellState::Alive);
        assert_eq!(rust_get_cell(0, 1), CellState::Alive);
    }

    #[test]
    fn stable_quad() {
        let _ =std::panic::catch_unwind(|| {
            rust_init(10, 10);
        });
        rust_set_cell(1, 0, CellState::Alive);
        rust_set_cell(0, 1, CellState::Alive);
        rust_set_cell(2, 1, CellState::Alive);
        rust_set_cell(1, 2, CellState::Alive);

        assert_eq!(rust_get_alive_neighbohrs(1, 0), 2);
        assert_eq!(rust_get_alive_neighbohrs(0, 1), 2);
        assert_eq!(rust_get_alive_neighbohrs(2, 1), 2);
        assert_eq!(rust_get_alive_neighbohrs(1, 2), 2);
        rust_tick();

        assert_eq!(rust_get_cell(1, 0), CellState::Alive);
        assert_eq!(rust_get_cell(0, 1), CellState::Alive);
        assert_eq!(rust_get_cell(2, 1), CellState::Alive);
        assert_eq!(rust_get_cell(1, 2), CellState::Alive);
    }
}
