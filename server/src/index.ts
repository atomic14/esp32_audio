import express from 'express';
import bodyParser from 'body-parser';
import fs from 'fs';

const app = express();
const port = 5003; // default port to listen

app.use(
  bodyParser.raw({
    // inflate: true,
    type: '*/*'
  })
);

// route to handle samples from the ADC - 16 bit single channel samples
app.post('/adc_samples', (req, res) => {
  // tslint:disable-next-line:no-console
  console.log(`Got ${req.body.length} ADC bytes`);
  fs.appendFile('adc.raw', req.body, () => {
    res.send('OK');
  });
});

// route to handle samples from the I2S microphones - 32 bit stereo channel samples
app.post('/i2s_samples', (req, res) => {
  // tslint:disable-next-line:no-console
  console.log(`Got ${req.body.length} I2S bytes`);
  fs.appendFile('i2s.raw', req.body, () => {
    res.send('OK');
  });
});

// start the Express server
app.listen(port, '0.0.0.0', () => {
  // tslint:disable-next-line:no-console
  console.log(`server started at http://0.0.0.0:${port}`);
});
