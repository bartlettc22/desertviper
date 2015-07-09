# NodeJS Application for Desert Viper Dashboard

## Installation/Setup 

### Install NodeJS on Windows
Download and install NodeJS from https://nodejs.org/download/

### Install NodeJS on Raspberry Pi
Download and install the latest NodeJS (ARM)
```
wget http://node-arm.herokuapp.com/node_latest_armhf.deb
sudo dpkg -i node_latest_armhf.deb
```

### Setup Application
Navigate to nodejs_app
```
cd <project_directory>/desertviper/nodejs_app/
```
On Linux, you may need to change the npm registry to the non-https version or some module installations may fail
```
npm config set registry http://registry.npmjs.org/
```
Install Nodemon and Bower globally (-g).  This will keep our application up if there are errors or if we change files. May need to run as sudo on Linux
```
~~npm install -g nodemon~~
npm install -g pm2
npm install -g bower
npm install -g browserify
```
Install required modules (these are already specified in package.json file)
```
npm install
```

### General NodeJS Nodes
To initialize a new NodeJS app, run the following and complete setup questions
```
npm init
bower init
```
To add a module run the following (the --save parameter saves the module as a dependancy in package.json)
```
npm install <module_name> --save
bower intstall <module_name>
```

## Run application
From app direction run
```
~~nodemon app.js~~
pm2 start app.js
pm2 list
```
