DELIMITER #

CREATE PROCEDURE IF NOT EXISTS ADD_NAME(IN ROLL INT, IN NM VARCHAR(20))
BEGIN
    INSERT INTO RESULT(S_ROLL, S_NAME)
    VALUES(ROLL, NM);
    INSERT INTO STUD_MARKS(S_ROLL)
    VALUES(ROLL);
END#

CREATE PROCEDURE IF NOT EXISTS ADD_RESULT(IN ROLL INT)
BEGIN
    DECLARE GRADE VARCHAR(20);-- Drop tables if they exist
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
(6, 'Frank', 990),DELIMITER //

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

(7, 'George', -1);


    DECLARE MARKS INT;

    SELECT S_MARKS INTO MARKS
    FROM STUD_MARKS 
    WHERE S_ROLL = ROLL;

    SET GRADE = (CASE 
    WHEN MARKS IS NULL THEN
        "MISSING"
    WHEN MARKS > 1500 THEN
        "IMPOSSIBLE"
    WHEN MARKS >= 990 THEN
        "DISTINCTION"
    WHEN MARKS >= 900 THEN
        "1ST CLASS"
    WHEN MARKS >= 825 THEN
        "HIGHER 2ND CLASS"
    WHEN MARKS < 400 THEN
        "FAIL"
    ELSE
        "PASS"
    END);

    UPDATE RESULT
    SET S_GRADE = GRADE
    WHERE S_ROLL = ROLL;
END#

CREATE PROCEDURE IF NOT EXISTS ADD_MARKS(IN ROLL INT, IN MARKS INT)
BEGIN
    UPDATE STUD_MARKS
    SET S_MARKS = MARKS
    WHERE S_ROLL = ROLL;
    CALL ADD_RESULT(ROLL);
END#

DELIMITER ;

CREATE TABLE STUD_MARKS (S_ROLL INT UNIQUE, S_MARKS INT);

CREATE TABLE RESULT (S_ROLL INT UNIQUE, S_NAME VARCHAR(20), S_GRADE VARCHAR(20));

CALL ADD_NAME(1,"Arfat");   CALL ADD_MARKS(1,1501);
CALL ADD_NAME(2,"ABC");     CALL ADD_MARKS(2,990);
CALL ADD_NAME(3,"PQR");     CALL ADD_MARKS(3,900);
CALL ADD_NAME(4,"XYZ");     CALL ADD_MARKS(4,825);
CALL ADD_NAME(5,"AAA");     CALL ADD_MARKS(5,400);
CALL ADD_NAME(6,"BBB");     CALL ADD_MARKS(6,399);
CALL ADD_NAME(7,"CCC");     CALL ADD_RESULT(7);

SELECT * FROM RESULT;

DROP TABLE STUD_MARKS;

DROP TABLE RESULT;

DROP PROCEDURE ADD_MARKS;

DROP PROCEDURE ADD_NAME;

DROP PROCEDURE ADD_RESULT;
