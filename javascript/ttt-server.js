const net = require('net');

(() => {
    // When the game is null we are waiting for
    // the first player to connect
    let game = null;

    net.createServer((socket) => {
        console.log('Connection from', socket.remoteAddress, 'port', socket.port);
        if (game == null) {
            game = new Game();
            game.playerX = new Player(game, socket, 'X');
        } else {
            game.playerO = new Player(game, socket, 'O');
            game = null; // set game = null to wait for other connections
        }
    }).listen(59080, () => {
        console.log('Tic Tac Toe Server is running');
    });
})();

class Game {
    // Each square is owned by a player, or null if no one has played there
    constructor() {
        this.board = Array(9).fill(null);
    }

    hasWinner() {
        const b = this.board;
        const wins = [[0,1,2],[3,4,5],[6,7,8],
                      [0,3,6],[1,4,7],[2,5,8],
                      [0,4,8],[2,4,6]];
        return wins.some(([x,y,z]) => b[x] != null && b[x] === b[y]
                                                   && b[y] === b[z]);
    }

    boardFilledUp() {
        return this.board.every(square => square != null);
    }

    move (location, player) {
        if (player !== this.currentPlayer) {
            throw new Error('Sit the fuck down it\'s not your turn');
        } else if (!player.opponent) {
            throw new Error('You don\'t have an opponent yet');
        } else if (this.board[location] !== null) {
            throw new Error('Cell already used');
        }
        this.board[location] = this.currentPlayer;
        this.currentPlayer = this.currentPlayer.opponent;
    }
}

class Player {
    constructor(game, socket, letter) {
        Object.assign(this, { game, socket, letter });
        this.send(`WELCOME ${letter}`);
        if (letter === 'X') {
            game.currentPlayer = this;
        } else {
            this.opponent = game.playerX;
            this.opponent.opponent = this;
            this.opponent.send('MESSAGE Your move.');
        }

        socket.on('data', (buffer) => {
            const cmd = buffer.toString('utf-8').trim();
            if (/^[q,Q]/.test(cmd)) {
                socket.destroy();
            } else if (/^[m,M]\s*\d+$/.test(cmd)) {
                const location = Number(cmd.slice(-1));
                try {
                    game.move(location, this);
                    this.send('Valid Move');
                    this.opponent.send(`Opponent Moved ${location}`);
                    if (this.game.hasWinner()) {
                        this.send('Victory');
                        this.opponent.send('Defeat');
                    } else if (this.game.boardFilledUp()) {
                        [this, this.opponent].forEach(p => p.send('Tie'));
                    }
                } catch (e) {
                    this.send(`Message ${e.message}`);
                }
            }
        });

        socket.on('close', () => {
            try { this.opponent.send('Other player left'); } catch (e) {}
        });
    }
    send(message) {
        this.socket.write(`${message}\n`);
    }
}
