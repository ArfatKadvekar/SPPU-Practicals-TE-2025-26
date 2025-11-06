// ===== 1️⃣ Create Collection =====
db.createCollection("Orders");

// ===== 2️⃣ Insert Documents =====
db.Orders.insertMany([
  { "_id": 1, "customer": "Alice", "amount": 250, "status": "Delivered" },
  { "_id": 2, "customer": "Bob", "amount": 150, "status": "Pending" },
  { "_id": 3, "customer": "Alice", "amount": 100, "status": "Delivered" },
  { "_id": 4, "customer": "Charlie", "amount": 200, "status": "Delivered" }
]);

// ===== 3️⃣ Map-Reduce Example =====

// Map function: emit amount per customer only for Delivered orders
var mapFunction = function() {
  if (this.status === "Delivered") {
    emit(this.customer, this.amount);
  }
};

// Reduce function: sum amounts per customer
var reduceFunction = function(key, values) {
  return Array.sum(values);
};

// Execute Map-Reduce
db.Orders.mapReduce(
  mapFunction,
  reduceFunction,
  { out: "totalPerCustomer" }
);

// ===== 4️⃣ View Map-Reduce Results =====
db.totalPerCustomer.find();
