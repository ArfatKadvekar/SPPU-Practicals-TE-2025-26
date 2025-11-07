-- 🧾 Assignment: Fine Calculation (PL/SQL / MySQL Stored Procedure)
-- Includes: Schema + Sample Data + Procedure + Example Execution
-- ---------------------------------------------------------------

-- 🧩 Step 1: Create Schema (Tables)
DROP TABLE IF EXISTS Fine;
DROP TABLE IF EXISTS Borrower;

CREATE TABLE Borrower (
    Roll_no INT PRIMARY KEY,
    Name VARCHAR(100),
    Date_of_Issue DATE,
    Name_of_Book VARCHAR(100),
    Status CHAR(1)        -- 'I' = Issued, 'R' = Returned
);

CREATE TABLE Fine (
    Roll_no INT,
    Date DATE,
    Amt DECIMAL(10,2),
    FOREIGN KEY (Roll_no) REFERENCES Borrower(Roll_no) ON DELETE CASCADE
);

-- 🧩 Step 2: Insert Sample Data
INSERT INTO Borrower VALUES
(101, 'Arjun',  '2025-10-01', 'DBMS', 'I'),
(102, 'Priya',  '2025-09-25', 'OS', 'I'),
(103, 'Riya',   '2025-11-01', 'CN', 'I'),
(104, 'Rohan',  '2025-09-15', 'SE', 'I');

-- 🧩 Step 3: Stored Procedure with Exception Handling
DROP PROCEDURE IF EXISTS CalculateFine;

DELIMITER //

CREATE PROCEDURE CalculateFine(IN p_Roll INT, IN p_Book VARCHAR(100))
BEGIN
    DECLARE issueDate DATE;
    DECLARE daysPassed INT;
    DECLARE fineAmt DECIMAL(10,2);

    -- Exception handlers
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        SELECT 'SQL Error occurred while calculating fine!' AS Message;
    END;

    DECLARE CONTINUE HANDLER FOR NOT FOUND
    BEGIN
        SELECT 'No such Borrower or Book found!' AS Message;
    END;

    -- Fetch the issue date
    SELECT Date_of_Issue INTO issueDate
    FROM Borrower
    WHERE Roll_no = p_Roll AND Name_of_Book = p_Book;

    -- Calculate fine if record found
    IF issueDate IS NOT NULL THEN
        SET daysPassed = DATEDIFF(CURDATE(), issueDate);

        IF daysPassed > 30 THEN
            SET fineAmt = daysPassed * 50;
        ELSEIF daysPassed BETWEEN 15 AND 30 THEN
            SET fineAmt = daysPassed * 5;
        ELSE
            SET fineAmt = 0;
        END IF;

        -- Update status to Returned
        UPDATE Borrower
        SET Status = 'R'
        WHERE Roll_no = p_Roll AND Name_of_Book = p_Book;

        -- Insert fine details if applicable
        IF fineAmt > 0 THEN
            INSERT INTO Fine VALUES (p_Roll, CURDATE(), fineAmt);
        END IF;

        SELECT CONCAT('Fine calculated successfully: Rs ', fineAmt) AS Message;
    ELSE
        SELECT 'Borrower or Book not found!' AS Message;
    END IF;
END //

DELIMITER ;

-- 🧩 Step 4: Execute Procedure (Test)
CALL CalculateFine(102, 'OS');

-- 🧩 Step 5: Check Results
SELECT * FROM Borrower;
SELECT * FROM Fine;
