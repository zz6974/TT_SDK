const { DatabaseSync } = require('node:sqlite');
const db = new DatabaseSync('D:/Project/Drone/data.db', { readOnly: true });
const tables = db.prepare("SELECT name FROM sqlite_master WHERE type='table'").all();
console.log('tables:', JSON.stringify(tables));
const rows = db.prepare('SELECT * FROM GPSData').all();
console.log('GPSData rows:', JSON.stringify(rows));
db.close();
