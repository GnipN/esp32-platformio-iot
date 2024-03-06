// Step 1: Initialize a new Node.js project
// This step is done in the terminal, not in the code

// Step 2: Install necessary dependencies
// This step is also done in the terminal, not in the code

// setup jwt
const jwt = require('jsonwebtoken');
const expressJwt = require('express-jwt');
const bcrypt = require('bcrypt');
const saltRounds = 10;
const cors = require('cors'); // allow Cross-domain requests
const crypto = require('crypto'); // for generating random token
// const secret = 'iot_secret_key'; // replace with your actual secret key
process.env.JWT_SECRET = 'iot_secret_key'; // !!! very important to set this env var and keep it secret (it should not be in your code repo)

// Step 3: Set up a connection to MongoDB using Mongoose
const mongoose = require('mongoose');
mongoose.connect('mongodb://usriot:passiot@mongo:27017/iot', {useNewUrlParser: true, useUnifiedTopology: true});

// Step 4: Define a Mongoose schema and model
const Schema = mongoose.Schema;
const UserSchema = new Schema({
  usr: String,
  pass: String,
  iotToken: String,
  status: { type: String, enum: ['active', 'inactive'], default: 'inactive' },
  created: { type: Date, default: Date.now }
});
const User = mongoose.model('User', UserSchema);

const sensValSchema = new Schema({
  token: String,
  sens_id: String,
  sens_val: String,
  created: { type: Date, default: Date.now }
});
const sensVal = mongoose.model('sensors_values', sensValSchema);

// Step 5: Create Express routes to handle CRUD operations 
// and use Mongoose to interact with the database 
const express = require('express');
const app = express();
app.use(express.json());
app.use(cors()); // allow Cross-domain requests

// Example route for user login
// app.post('/login', (req, res) => {
//   // Authenticate user credentials
//   if (isValidUser(req.body.username, req.body.password)) {
//     const payload = { userId: 1 }; // Replace with actual user data
//     const token = jwt.sign(payload, process.env.JWT_SECRET, { expiresIn: '1h' });
//     res.json({ token });
//   } else {
//     res.status(401).json({ message: 'Invalid credentials' });
//   }
// });

async function createUser(usr, pass) {
  // Hash the password
  const hashedPassword = await bcrypt.hash(pass, saltRounds);

  // Create a token for the embedded devices connection
  const iotToken = crypto.randomBytes(30).toString('hex');

  // Create a new user
  const user = new User({
    usr: usr,
    pass: hashedPassword,
    iotToken: iotToken
  });

  // Save the user to the database
  const result = await user.save();

  return result;
}

// Example route for user signup
app.post('/signup', async (req, res) => {
  try {
  // Check if the username is already taken
  const { usr, pass } = req.body;
  const existingUser = await User.findOne({ usr });
  if (existingUser) {
    return res.status(400).json({ message: 'Username is already taken' });
  }

  
  const user = await createUser(usr, pass);
  // res.json(user);
  res.status(200).json({ message: 'User created' });
} catch (error) {
  // Log the error and return a 500 status code
  console.error(error);
  res.status(500).json({ message: 'signup Internal server error' });
}
});

app.post('/login', async (req, res) => {
  try {
  const { usr, pass } = req.body;
  // Query the user from the database
  const user = await User.findOne({ usr });

  // If user doesn't exist, send an error response
  if (!user) {
    return res.status(400).json({ message: 'Invalid username or password' });
  }

  // Check if the provided password matches the hashed password in the database
  const validPassword = await bcrypt.compare(pass, user.pass);

  // If password doesn't match, send an error response
  if (!validPassword) {
    return res.status(400).json({ message: 'Invalid username or password' });
  }


  const token = jwt.sign({ usr }, process.env.JWT_SECRET, { expiresIn: '1h' });
  return res.status(200).json({ message: 'Login success', iotToken: user.iotToken, jwtToken: token });
} catch (error) {
  console.error(error);
  res.status(500).json({ message: 'login Internal server error' });
}
});

// app.use(expressJwt({ secret, algorithms: ['HS256'] }).unless({ path: ['/login'] }));

// app.get('/protected', expressJwt({ secret }), (req, res) => {});

// Middleware to verify JWT
function verifyToken(req, res, next) {
  const authHeader = req.headers['authorization'];
  const token = authHeader && authHeader.split(' ')[1];

  if (!token) {
    return res.status(401).json({ message: 'Unauthorized' });
  }

  jwt.verify(token, process.env.JWT_SECRET, (err, decoded) => {
    if (err) {
      return res.status(403).json({ message: 'Forbidden' });
    }
    req.user = decoded; // Attach decoded user data to request
    next();
  });
}

// Protected route
// To test this in Postman, get token from /login route with Body is {"username":"usr", "password":"pass"} 
// then add given token to the Authorization header by selecting "Bearer Token" as the type 
// and pasting the token into the Token field
app.get('/protected', verifyToken, (req, res) => {
  res.json({ message: 'Protected route accessed' });
});


app.get('/test', (req, res) => {
  const { usr, pass } = req.body;
  res.send('success usr:' + usr + ' pass:' + pass);
});

app.get('/users', async (req, res) => {
  const users = await User.find();
  res.send(users);
});

app.post('/users', async (req, res) => {
  const user = new User(req.body);
  await user.save();
  res.send(user);
});

app.put('/users/:id', async (req, res) => {
  await User.findByIdAndUpdate(req.params.id, req.body);
  const user = await User.findById(req.params.id);
  res.send(user);
});

app.delete('/users/:id', async (req, res) => {
  await User.findByIdAndDelete(req.params.id);
  res.send('Deleted');
});

// Example route for creating a new sensor value
app.post('/sensVal', async (req, res) => {
  const { token, sens_id, sens_val } = req.body;

  // Create a new sensor value
  const newSensVal = new sensVal({
    token: token,
    sens_id: sens_id,
    sens_val: sens_val
  });

  // Save the sensor value to the database
  const result = await newSensVal.save();

  // Send the result back to the client
  res.json(result);
});

// Step 6: Export the Express app
// module.exports = app;

app.listen(3000, () => {
 console.log('Server started on port 3000');
});