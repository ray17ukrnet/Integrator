DELIMITER $$

DROP PROCEDURE IF EXISTS `tradedb3_1`.`ag_IntegSetTransactionCommStatus` $$
CREATE DEFINER=`TradeServer`@`%` PROCEDURE `ag_IntegSetTransactionCommStatus`(
  IN inIdComBill bigint,
  IN inPayDateTime datetime,
  IN inUniKioskStatus int
)
BEGIN

  DECLARE exec_result int;
  DECLARE error_msg   char(80);

  DECLARE locIdComBill      bigint;
  -- DECLARE outUniKioskStatus int;
  -- DECLARE outPayDateTime    datetime;
  DECLARE locDateOp            datetime;
  DECLARE locPayed             datetime;
  DECLARE locDateOpCalculated  datetime;
  -- ----------------
  SET exec_result = 0;
  SET error_msg = 'No actions.';
  -- ----------------
  -- SET outPayDateTime    = inPayDateTime;
  -- SET outUniKioskStatus = inUniKioskStatus;
  -- ----------------
  SELECT IdComBill
  INTO   locIdComBill
  FROM   TransactionsComm
  WHERE  IdComBill = inIdComBill;
  -- ----------------
  IF ISNULL(locIdComBill) THEN
    BEGIN
      SET exec_result = 1;
      SET error_msg   = 'Транзакцiя вiдсутня у TransactionsComm.';
    END;
  ELSE
    BEGIN
      SELECT DateOp,    Payed
      INTO   locDateOp, locPayed
      FROM TransactionsComm
      WHERE IdComBill = inIdComBill;

      -- ----------------
      IF inPayDateTime >= locPayed THEN
        BEGIN
          SET locDateOpCalculated = inPayDateTime;
        END;
      END IF;
      -- ----------------
      IF inPayDateTime < locPayed THEN
        BEGIN
          SET locDateOpCalculated = Now();
        END;
      END IF;
      -- ----------------
      UPDATE TransactionsComm
      SET DateOp            = locDateOpCalculated,
          Fulfilled         = inPayDateTime,
          TransactionStatus = inUniKioskStatus
      WHERE IdComBill = inIdComBill;
      -- ----------------
      --  IF ISNULL(locDateOp) THEN BEGIN END; ELSE BEGIN END; END IF;
      SET exec_result = 0;
      SET error_msg   = 'Fulfilled, TransactionStatus, DateOp оновлено у TransactionsComm успiшно.';
      -- ----------------
    END;
  END IF;
  -- ----------------
/*
      SELECT DateOp,    Payed
      INTO   locDateOp, locPayed
      FROM TransactionsComm
      WHERE IdComBill = inIdComBill;
      -- locDateOpCalculated
      IF ISNULL(locDateOp) THEN
        BEGIN
          -- ----------------
          IF inPayDateTime >= locPayed THEN
            BEGIN
              SET locDateOpCalculated = inPayDateTime;
            END;
          END IF;
          -- ----------------
          IF inPayDateTime < locPayed THEN
            BEGIN
              SET locDateOpCalculated = Now();
            END;
          END IF;
          -- ----------------
          UPDATE TransactionsComm
          SET DateOp = locDateOpCalculated
          WHERE IdComBill = inIdComBill;
          -- ----------------
        END;
      -- ELSE BEGIN END;
      END IF;
*/
  -- ----------------
  SELECT exec_result, error_msg;

END $$

DELIMITER ;