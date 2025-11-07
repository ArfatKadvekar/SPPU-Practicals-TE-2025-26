-- 🧾 Assignment: Calculate Area of Circle using Stored Procedure
-- Includes: Schema + Procedure + Exception Handling + Execution
-- ---------------------------------------------------------------

-- 🧩 Step 1: Create Table
DROP TABLE IF EXISTS areas;

CREATE TABLE areas (
    radius INT,
    area DECIMAL(10,2)
);

-- 🧩 Step 2: Create Stored Procedure
DROP PROCEDURE IF EXISTS CalculateCircleArea;

DELIMITER //

CREATE PROCEDURE CalculateCircleArea()
BEGIN
    DECLARE r INT DEFAULT 5;        -- starting radius
    DECLARE a DECIMAL(10,2);        -- area variable

    -- Exception Handler: if any SQL error occurs
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        SELECT '⚠️ SQL Error occurred while calculating area!' AS Message;
    END;

    -- 🌀 Loop from radius 5 to 9
    WHILE r <= 9 DO
        SET a = 3.14 * r * r;       -- formula: πr²
        INSERT INTO areas VALUES (r, a);
        SET r = r + 1;
    END WHILE;

    SELECT '✅ Area calculation completed successfully!' AS Message;
END //

DELIMITER ;

-- 🧩 Step 3: Execute the Procedure
CALL CalculateCircleArea();

-- 🧩 Step 4: View Results
SELECT * FROM areas;
