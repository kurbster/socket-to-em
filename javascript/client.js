const net = require('net');
const readline = require('readline');

const client = new net.Socket();
client.connect(59080, process.argv[2], () => {
    console.log('Connected to server');
});
client.on('data', (data) => {
    console.log(data.toString('utf-8'));
});

const rl = readline.createInterface({ input: process.stdin });
rl.on('line', (line) => {
    client.write(`${line}\n`);
});
rl.on('close', () => {
    client.end();
});
