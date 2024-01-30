use std::sync::{Mutex, MutexGuard, OnceLock};

#[derive(Copy, Clone)]
pub enum CellState {
    Alive,
    Dead,
}

struct Board {
    cells: Vec<Vec<CellState>>,
}

static BOARD:OnceLock<Mutex<Board>> = OnceLock::new();

impl Board {

    fn instance() -> MutexGuard<'static, Board>{
        //BOARD.get().expect("Board not initialized").lock().unwrap()
        BOARD.get().expect("Cannot get board").lock().unwrap_or_else(|_| panic!("Cannot unlock board"))
    }

    fn initialize(width: usize, height: usize){
        let cells = vec![vec![CellState::Dead; width]; height];
        let board = Board { cells };
        let mutex: Mutex<Board> = Mutex::new(board);
        BOARD.set(mutex).unwrap_or_else(|_| panic!("Board already initialized"));
    }
}

pub fn init(width: usize, height: usize) {
    Board::initialize(width, height);
    Board::instance().cells[0][0] = CellState::Alive;
}

pub fn get_cell(x: usize, y: usize) -> CellState {
    Board::instance().cells[x][y]
}

pub fn set_cell(x: usize, y: usize, state: CellState) {
    Board::instance().cells[x][y] = state;
}



#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn can_initialize() {
        init(10, 10);
    }

    #[test]    
    #[should_panic(expected = "Board already initialized")]
    fn cannot_reinitialize() {
        init(10, 10);        
        init(10, 10);
    }
}
