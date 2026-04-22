DELIMITER $$

DROP PROCEDURE IF EXISTS `tradedb3_1`.`ag_AddToPayOwner` $$
CREATE PROCEDURE `tradedb3_1`.`ag_AddToPayOwner` (
  IN inIdcombill BIGINT(20),
  IN inIdbuyer   INTEGER,
  IN inIdservice INTEGER,
  IN inPaydate   DATETIME,
  IN inPerson    VARCHAR(128),
  IN inAddress   VARCHAR(128),
  IN inOther     VARCHAR(128)
)
BEGIN

  DECLARE exec_result  int;
  DECLARE error_msg    char(80);
  DECLARE locIdComBill BIGINT(20);
  -- -------------------------
  SET exec_result  = 0;
  SET error_msg    = 'No actions. ';
  SET locIdComBill = 0;
  -- -------------------------
  SELECT IdComBill
  INTO   locIdComBill
  FROM   PayOwner
  WHERE  IdComBill = inIdComBill;
  -- -------------------------
  IF ISNULL(locIdComBill) OR locIdComBill = 0 THEN
    BEGIN -- ELSE
      INSERT PayOwner (
        idcombill, idbuyer, idservice, paydate, person, address, other
      )
      VALUES (
        inIdcombill,
        inIdbuyer,
        inIdservice,
        inPaydate,
        inPerson,
        inAddress,
        inOther
      );
      -- -------------------------
      SET exec_result = 0;
      SET error_msg   = CONCAT('Insert into PayOwner successfully. IdComBill=', CAST(inIdComBill AS char) );
      -- -------------------------
    END;
  ELSE
    BEGIN
      SET error_msg = CONCAT('Record already present in table PayOwner. IdComBill=', CAST(inIdComBill AS char) );
    END;
  END IF; -- IF ISNULL(locIdUBill) OR locIdUBill = 0 THEN
  -- -------------------------
  SELECT
    exec_result,
    error_msg;
  -- -------------------------

END $$

DELIMITER ;