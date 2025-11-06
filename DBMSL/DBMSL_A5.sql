-- Drop tables if they exist
DROP TABLE IF EXISTS Students;
DROP TABLE IF EXISTS Results;

-- Students table
CREATE TABLE Students (
    rollNo INT PRIMARY KEY CHECK (rollNo > 0),
    nameOfStud VARCHAR(500) NOT NULL,
    totalMarks INT NOT NULL DEFAULT 0
);

-- Results table
CREATE TABLE Results (
    studentId INT PRIMARY KEY,
    clas VARCHAR(50),
    FOREIGN KEY (studentId) REFERENCES Students(rollNo) ON DELETE CASCADE
);

-- Sample data
INSERT INTO Students (rollNo, nameOfStud, totalMarks)
VALUES 
(1, 'Alice', 995),
(2, 'Bob', 920),
(3, 'Charlie', 840),
(4, 'David', 700),
(5, 'Eva', 1501),
(6, 'Frank', 990),
(7, 'George', -1);

DELIMITER //

DROP PROCEDURE IF EXISTS proc_grade;

CREATE PROCEDURE proc_grade(
    IN p_roll INT,
    IN p_name VARCHAR(100)
)
BEGIN
    DECLARE marks INT;
    DECLARE clasI VARCHAR(50);

    -- Handler for unexpected SQL errors
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        SELECT 'Error: Something went wrong with the database operation.' AS Message;
    END;

    -- Validate roll number
    IF p_roll <= 0 THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Error: Invalid roll number';
    END IF;

    -- Validate student name
    IF p_name IS NULL OR p_name = '' THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Error: Invalid student name';
    END IF;

    -- Get student marks
    SELECT totalMarks INTO marks
    FROM Students
    WHERE rollNo = p_roll AND nameOfStud = p_name;

    -- Check if student exists
    IF marks IS NULL THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Error: Student not found';
    END IF;

    -- Validate marks
    IF marks < 0 OR marks > 1500 THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Error: Invalid marks';
    END IF;

    -- Categorize marks
    IF marks >= 990 THEN
        SET clasI = 'Distinction';
    ELSEIF marks >= 900 THEN
        SET clasI = 'First Class';
    ELSEIF marks >= 825 THEN
        SET clasI = 'Higher Second Class';
    ELSE
        SET clasI = 'Fail';
    END IF;

    -- Insert into Results
    INSERT INTO Results(studentId, clas)
    VALUES (p_roll, clasI);

    SELECT CONCAT('Success: Result for ', p_name, ' calculated as ', clasI) AS Message;

END;
//

DELIMITER ;
