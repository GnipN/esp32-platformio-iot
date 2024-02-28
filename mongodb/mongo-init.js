// ref: https://stackoverflow.com/questions/42912755/how-to-create-a-db-for-mongodb-container-on-start-up

// connection string for MongoDB Compass
// mongodb://usriot:passiot@192.168.74.251:27017/iot

// need to add gitignore at mongodb/data/

db.createUser(
  {
      user: "usriot",
      pwd: "passiot",
      roles: [
          {
              role: "readWrite",
              db: "iot"
          }
      ]
  }
);