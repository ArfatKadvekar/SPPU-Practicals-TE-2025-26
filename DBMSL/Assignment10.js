// ===== 1️⃣ Create Collection =====
db.createCollection("Orders");

// ===== 2️⃣ Insert Documents =====
db.Orders.insertMany([
  { "_id": 1, "customer": "Alice", "amount": 250, "status": "Delivered", "date": "2025-11-01" },
  { "_id": 2, "customer": "Bob", "amount": 150, "status": "Pending", "date": "2025-11-02" },
  { "_id": 3, "customer": "Charlie", "amount": 200, "status": "Delivered", "date": "2025-11-03" },
  { "_id": 4, "customer": "David", "amount": 300, "status": "Delivered", "date": "2025-11-04" },
  { "_id": 5, "customer": "Alice", "items": [
      { "product": "Laptop", "qty": 1 },
      { "product": "Mouse", "qty": 2 }
    ]
  }
]);

// ===== 3️⃣ Create Indexes =====
db.Orders.createIndex({ customer: 1 });
db.Orders.createIndex({ status: 1 });
db.Orders.createIndex({ date: 1 });

// ===== 4️⃣ Aggregation Examples =====

// ---- 4a. $match + $project ----
db.Orders.aggregate([
  { $match: { status: "Delivered" } },
  { $project: { _id: 0, customer: 1, amount: 1, status: 1 } }
]);

// ---- 4b. $group + $sort ----
db.Orders.aggregate([
  { $group: { _id: "$customer", totalAmount: { $sum: "$amount" } } },
  { $sort: { totalAmount: -1 } }
]);

// ---- 4c. $unwind + $addFields ----
db.Orders.aggregate([
  { $unwind: "$items" },
  { $addFields: { totalQty: { $sum: "$items.qty" } } } // total quantity per order
]);

// ---- 4d. $match + $count ----
db.Orders.aggregate([
  { $match: { amount: { $gt: 150 } } },
  { $count: "highValueOrders" }
]);

// ---- 4e. $sort + $limit ----
db.Orders.aggregate([
  { $sort: { amount: -1 } },  // Sort descending by amount
  { $limit: 3 }               // Top 3 orders
]);

// ---- 4f. $sample + $limit ----
db.Orders.aggregate([
  { $sample: { size: 2 } },  // Random 2 documents
  { $limit: 1 }              // Take only 1 document
]);
