const http = require('http');
 
const hostname = '127.0.0.1';
const port = 3000;

const {SerialPort} = require('serialport');
const {ReadlineParser}= require('@serialport/parser-readline');
const comPort = new SerialPort({
  path: 'COM4',
  baudRate: 115200,
  dataBits: 8,
  stopBits: 1,
  parity: 'none',
  });

const parser = comPort.pipe(new ReadlineParser({ delimiter: '\n' }));
// Read the port data
comPort.on("open", () => {
  console.log('serial port open');
});
parser.on('data', data =>{
  console.log('got word from arduino:', data);
});


const server = http.createServer((req, res) => {
  res.statusCode = 200;
  res.setHeader('Content-Type', 'text/plain');
  res.end('Hello World');
});


 
server.listen(port, hostname, () => {
  console.log(`Server running at http://${hostname}:${port}/`);
});
