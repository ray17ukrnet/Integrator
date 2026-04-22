DELIMITER $$

DROP PROCEDURE IF EXISTS `tradedb3_1`.`ag_AddToPayOwner_v2` $$
CREATE DEFINER=`TradeServer`@`%` PROCEDURE `ag_AddToPayOwner_v2`(
  IN inIdcombill BIGINT(20),
  IN inIdbuyer   INTEGER,
  IN inIdservice INTEGER,
  IN inAmount    DECIMAL(12,4),
  IN inPaydate   DATETIME,
  IN inPerson    VARCHAR(128),
  IN inAddress   VARCHAR(128),
  IN inOther     VARCHAR(128),
  IN inPayType   INTEGER,
  IN inPayName   VARCHAR(45)
)
BEGIN
  DECLARE Result  int;
  DECLARE exec_result  int;
  DECLARE error_msg    char(80);
  DECLARE locIdComBill BIGINT(20);
  -- -------------------------
  SET Result       = 0;
  SET exec_result  = 0;
  -- exec_result = 1 - wrong INN, exec_result = 2 - wrong IBAN, exec_result = 3 - NULL date of birth
  -- // ІПН| дата народження (dd.mm.yyyy) | місце народження | телефон |серія номер паспорта (Idкартки)|дата видачі парспорта.
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
        idcombill, idbuyer, idservice, Amount, paydate, person, address, other, paytype, payname
      )
      VALUES (
        inIdcombill,
        inIdbuyer,
        inIdservice,
        inAmount,
        inPaydate,
        inPerson,
        inAddress,
        inOther,
        inPayType,
        inPayName
      );
      -- -------------------------
      SET exec_result = 0;
      SET error_msg   = CONCAT('Inserted into PayOwner successfully. IdComBill=', CAST(inIdComBill AS char) );
      -- -------------------------
    END;
  ELSE
    BEGIN
      SET error_msg = CONCAT('Record already present in table PayOwner. IdComBill=', CAST(inIdComBill AS char) );
    END;
  END IF; -- IF ISNULL(locIdUBill) OR locIdUBill = 0 THEN
  -- -------------------------
  SELECT
    result,
    exec_result,
    error_msg;
  -- -------------------------
END $$

DELIMITER ;