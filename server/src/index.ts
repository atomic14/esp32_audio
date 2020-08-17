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

// define a route handler for the default home page
app.post('/samples', (req, res) => {
  // tslint:disable-next-line:no-console
  console.log(`Got ${req.body.length} bytes`);
  fs.appendFile('audio.raw', req.body, () => {
    res.send('OK');
  });
});

// start the Express server
app.listen(port, '0.0.0.0', () => {
  // tslint:disable-next-line:no-console
  console.log(`server started at http://0.0.0.0:${port}`);
});
