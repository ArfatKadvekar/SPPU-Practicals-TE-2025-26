-- 🧾 Assignment: Trigger on Library (Track Updates & Deletes)
-- -----------------------------------------------------------

DROP TABLE IF EXISTS Library_Audit;
DROP TABLE IF EXISTS Library;

-- Step 1️⃣: Create main table
CREATE TABLE Library (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100)
);

-- Step 2️⃣: Create audit table
CREATE TABLE Library_Audit (
    audit_id INT AUTO_INCREMENT PRIMARY KEY,
    id INT,
    book_name VARCHAR(100),
    action_type VARCHAR(20)
);

-- Step 3️⃣: Insert sample data
INSERT INTO Library (name) VALUES ('DBMS'), ('C Programming'), ('OS');

-- Step 4️⃣: BEFORE DELETE Trigger
DELIMITER //
CREATE TRIGGER before_delete_lib
BEFORE DELETE ON Library
FOR EACH ROW
BEGIN
    INSERT INTO Library_Audit (id, book_name, action_type)
    VALUES (OLD.id, OLD.name, 'BEFORE DELETE');
END;
//

-- Step 5️⃣: AFTER UPDATE Trigger
CREATE TRIGGER after_update_lib
AFTER UPDATE ON Library
FOR EACH ROW
BEGIN
    INSERT INTO Library_Audit (id, book_name, action_type)
    VALUES (NEW.id, NEW.name, 'AFTER UPDATE');
END;
//
DELIMITER ;

-- Step 6️⃣: Test the triggers
UPDATE Library SET name = 'DBMS 2' WHERE id = 1;
DELETE FROM Library WHERE id = 2;

-- Step 7️⃣: Check results
SELECT * FROM Library;
SELECT * FROM Library_Audit;
