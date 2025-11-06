// ===== 1️⃣ Create Collection =====
db.createCollection("Students");

// ===== 2️⃣ Insert One Student =====
db.Students.insertOne({ roll: 1, name: "Alice", marks: 995 });

// ===== 3️⃣ Insert Many Students =====
db.Students.insertMany([
  { roll: 2, name: "Bob", marks: 920 },
  { roll: 3, name: "Charlie", marks: 840 },
  { roll: 4, name: "David", marks: 700 },
  { roll: 5, name: "Eva", marks: 1501 },   // Invalid marks
  { roll: 6, name: "Frank", marks: 990 },
  { roll: 7, name: "George", marks: -1 }  // Invalid marks
]);

// ===== 4️⃣ Assign Class Using UpdateMany and Logical Operators =====

// Distinction: marks >= 990 and <=1500
db.Students.updateMany(
  { $and: [ { marks: { $gte: 990 } }, { marks: { $lte: 1500 } } ] },
  { $set: { class: "Distinction" } }
);

// First Class: 900-989
db.Students.updateMany(
  { $and: [ { marks: { $gte: 900 } }, { marks: { $lt: 990 } } ] },
  { $set: { class: "First Class" } }
);

// Higher Second Class: 825-899
db.Students.updateMany(
  { $and: [ { marks: { $gte: 825 } }, { marks: { $lt: 900 } } ] },
  { $set: { class: "Higher Second Class" } }
);

// Fail: 0-824
db.Students.updateMany(
  { $and: [ { marks: { $gte: 0 } }, { marks: { $lt: 825 } } ] },
  { $set: { class: "Fail" } }
);

// Invalid Marks: <0 or >1500
db.Students.updateMany(
  { $or: [ { marks: { $lt: 0 } }, { marks: { $gt: 1500 } } ] },
  { $set: { class: "Invalid Marks" } }
);

// ===== 5️⃣ Queries Using Logical Operators =====

// All students
db.Students.find().pretty();

// Students with marks >=900 AND class "First Class"
db.Students.find({ $and: [ { marks: { $gte: 900 } }, { class: "First Class" } ] }).pretty();

// Students with class "Fail" OR marks < 850
db.Students.find({ $or: [ { class: "Fail" }, { marks: { $lt: 850 } } ] }).pretty();

// ===== 6️⃣ Update Example =====
db.Students.updateOne({ roll: 3 }, { $set: { marks: 910, class: "First Class" } });

// ===== 7️⃣ Delete Example =====
db.Students.deleteMany({ class: "Invalid Marks" });
