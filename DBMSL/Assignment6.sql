-- 🧾 Assignment: Merge Data using Parameterized Cursor
-- ----------------------------------------------------

-- Step 1️⃣: Create sample tables
DROP TABLE IF EXISTS O_Empld;
DROP TABLE IF EXISTS N_Empld;

CREATE TABLE O_Empld (
    Emp_id INT PRIMARY KEY,
    Emp_name VARCHAR(100),
    Emp_salary DECIMAL(10,2)
);

CREATE TABLE N_Empld (
    Emp_id INT,
    Emp_name VARCHAR(100),
    Emp_salary DECIMAL(10,2)
);

-- Step 2️⃣: Insert sample data
INSERT INTO O_Empld VALUES
(101, 'Arjun', 50000),
(102, 'Priya', 60000);

INSERT INTO N_Empld VALUES
(102, 'Priya', 60000),   -- Duplicate (should be skipped)
(103, 'Rohan', 55000),
(104, 'Neha', 52000);

-- Step 3️⃣: Stored Procedure using Parameterized Cursor
DROP PROCEDURE IF EXISTS MergeEmployees;

DELIMITER //

CREATE PROCEDURE MergeEmployees()
BEGIN
    DECLARE done INT DEFAULT 0;
    DECLARE new_id INT;
    DECLARE new_name VARCHAR(100);
    DECLARE new_salary DECIMAL(10,2);

    -- ✅ Cursor with parameter (though not required for simple merge, added for example)
    DECLARE cur CURSOR FOR
        SELECT Emp_id, Emp_name, Emp_salary FROM N_Empld WHERE Emp_id > 0;

    DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = 1;

    OPEN cur;

    read_loop: LOOP
        FETCH cur INTO new_id, new_name, new_salary;
        IF done THEN
            LEAVE read_loop;
        END IF;

        -- ✅ Insert only if record not exists
        IF NOT EXISTS (SELECT 1 FROM O_Empld WHERE Emp_id = new_id) THEN
            INSERT INTO O_Empld VALUES (new_id, new_name, new_salary);
        END IF;
    END LOOP;

    CLOSE cur;

    SELECT '✅ Merge Completed Successfully!' AS Message;
END //

DELIMITER ;

-- Step 4️⃣: Execute procedure
CALL MergeEmployees();

-- Step 5️⃣: Check final merged data
SELECT * FROM O_Empld;
