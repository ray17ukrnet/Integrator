DELIMITER $$

DROP PROCEDURE IF EXISTS `tradedb3_1`.`CommonProcess113Stage2_v7` $$
CREATE PROCEDURE `tradedb3_1`.`CommonProcess113Stage2_v7` (
IN inIdComBill bigint,
IN inIdMillRq bigint,
IN inIdTransaction bigint,
IN inExtReqNo varchar(40),
IN inIdService int,
IN inIdSubService int,
IN inIdClient varchar(128),
IN inName varchar(50),
IN inAmountPayed numeric(10, 2),
IN inAmountCommis numeric(10, 2),
IN inIdCompany int,
IN inIdTerm varchar(50),
IN inKeyValue varchar(12),
IN inPayDateTime datetime,
IN inMillOperationId bigint,
IN inMillAcceptTime datetime,
IN inMillAcceptStatus tinyint,
IN inProviderAcceptStatus tinyint,
IN inProviderExtNo bigint,
IN inUpdateMillAcceptTime int,
IN inUniKioskStatus int,

OUT outUniKioskStatus int,
OUT outPayDateTime datetime,
OUT outReturnVoucher tinyint,
OUT outSendToOvertaking tinyint,
OUT exec_result int,
OUT error_msg char(80))
main:
BEGIN
  -- Store: D:\JOB\PROJECTS\VC10_Transition\PROJECTS\IntegrateUniInterface\Project\UniServiceAndHttp\UniService\DB\UniserviceBill\CommonProcess113Stage2_v7.sql
  -- inProviderAcceptStatus
  --  pasUnknown = -1,									// неизвестный
  --  pasOK = 0,                        // счет успешно оплачен
  --  pasTimeout = 23,								  // Таймаут при обращении к провайдеру
  --  pasOutOfMoney = 17,						  	// закончились наши деньги у провайдера
  --  pasUnknownClient = 15,            // неизвестный клиент
  --  pasError = 24									    // постоянная ошибка провайдера

  -- outReturnVoucher 1 - return 0 - dont return
  -- outSendToOvertaking 1 - send 0 - dont send

  -- inIdMillRq = =1 - use auto increment for table
  -- inIdTransaction = -1 - use auto increment for table

  --  DECLARE CurIdComBill bigint;
  DECLARE curIdBuyer smallint;
  DECLARE curBuyerType smallint;
  DECLARE curVoucherIdComBill bigint;
  DECLARE curFCVoucherStatus tinyint;
  DECLARE curTransactionStatus tinyint;
  DECLARE newTransactionStatus tinyint;
  DECLARE curTransactionAmount numeric(10, 2);
  DECLARE curTransactionCommis numeric(10, 2);
  DECLARE curOurCommRate numeric(12, 4);
  DECLARE curByuerCommRate numeric(12, 4);
  DECLARE newCommissionAmt numeric(12, 4);
  --  DECLARE newCommissionBank numeric(12, 4);
  DECLARE curIsMillOvertaking smallint;
  DECLARE curIsProviderOvertaking smallint;
  DECLARE curIsServiceProviderOvertaking smallint;
  DECLARE newFinishDate datetime;

  --
  DECLARE locDateOp                      datetime;
  DECLARE locDateOpCalculated            datetime;
  DECLARE locPayed                       datetime;


  SET exec_result = 0;
  SET error_msg = '';
  SET outUniKioskStatus = inUniKioskStatus;
  SET outPayDateTime = inPayDateTime;
  --  SET outChequeText = '';
  SET outReturnVoucher = 1;
  SET outSendToOvertaking = 0;

  SET newFinishDate = DATE_ADD(CURDATE(), INTERVAL 1 DAY);

  /*
  -- check exists comm bill id
  SELECT
    IdComBill INTO CurIdComBill
  FROM CommBills
  WHERE IdComBill = InIdComBill;
  */

  -- DEBUG >>>
  --  SELECT 1;
  -- <<< DEBUG 

  -- select buyer by key
  SELECT
    IdBuyer INTO curIdBuyer
  FROM SignKeys
  WHERE KeyValue = InKeyValue
  AND KeyStatus = 0;

  -- DEBUG >>>
  --  SELECT 2;
  -- <<< DEBUG 

  -- select status of certificate
  SELECT
    IdComBill,
    MillRqStatus INTO curVoucherIdComBill, curFCVoucherStatus
  FROM MillTx
  WHERE ExtReqNo = inExtReqNo
  AND IdBuyer = curIdBuyer;

  -- DEBUG >>>
  --  SELECT 3;
  -- <<< DEBUG 

  -- select from table TransactionsComm
  SELECT
    TransactionStatus,
    Amount,
    CommissionAmt INTO curTransactionStatus, curTransactionAmount, curTransactionCommis
  FROM TransactionsComm
  WHERE IdComBill = inIdComBill;

  -- DEBUG >>>
  --  SELECT 4;
  -- <<< DEBUG 

  -- select buyer type
  SELECT
    BuyerType,
    StMill13,
    StProvider INTO curBuyerType, curIsMillOvertaking, curIsProviderOvertaking
  FROM Buyers
  WHERE IdBuyer = curIdBuyer;

  /*
  SELECT ChequeText
  INTO
    outChequeText
  FROM
    ChequeInfo
  WHERE
    IdService = inIdService
    AND BuyerType = curBuyerType;
*/

  -- DEBUG >>>
  --  SELECT 5;
  -- <<< DEBUG 

  -- check exist of commission for service
  SELECT
    OurCommRate,
    stProvider INTO curOurCommRate, curIsServiceProviderOvertaking
  FROM ServiceComm
  WHERE IdService = InIdService;

  -- DEBUG >>>
  --  SELECT 6;
  -- <<< DEBUG 

  -- select our commission rate
  -- check exist of commission for buyer
  SELECT
    ByuerCommRate INTO curByuerCommRate
  FROM TxcommComm
  WHERE IdBuyer = curIdBuyer
  AND Service = InIdService;

  -- set provide status overtaking
  IF curIsServiceProviderOvertaking = 1 AND curIsProviderOvertaking = 1 THEN
    SET curIsProviderOvertaking = 1;
  ELSE
    SET curIsProviderOvertaking = 0;
  END IF;

  /*
  -- check if IdComBill exists 
  IF ISNULL(CurIdComBill) THEN
  BEGIN
    SET outUniKioskStatus = 19;
    SET outReturnVoucher = 1;
    SET exec_result = 0;
    SET error_msg = 'Absent or unknown - IdComBill';
  END;
  
  ELSE
    */
  -- check givc bill status
  IF curTransactionStatus = 0 AND inProviderAcceptStatus = 0 THEN
  BEGIN
    SET outUniKioskStatus = 11;
    SET outReturnVoucher = 0;
    SET exec_result = 0;
    SET error_msg = 'Bill already payed';
  END;
  -- out of money
  ELSE
    IF inMillAcceptStatus = 10 THEN
    BEGIN
      SET outUniKioskStatus = 16;
      SET outReturnVoucher = 0;
      SET exec_result = 0;
      SET error_msg = 'Out of money';
    END;
    -- check voucher status
    ELSE
      IF inMillAcceptStatus <> 0 THEN
      BEGIN
        IF curIsMillOvertaking = 1 THEN
          SET outUniKioskStatus = 13;
          SET outSendToOvertaking = 1;
        ELSE
          SET outUniKioskStatus = 21;
        END IF;
        SET outReturnVoucher = 0;
        SET exec_result = 0;
        SET error_msg = CONCAT(
        'Invalid mill status = '
        , CAST(inMillAcceptStatus AS char)
        );
      END;
      -- check amount 
      ELSE
        IF curTransactionAmount <> InAmountPayed THEN
        BEGIN
          SET outUniKioskStatus = 23;
          SET outReturnVoucher = 0;
          SET exec_result = 0;
          SET error_msg = CONCAT(
          'inAmountPayed = '
          , CAST(inAmountPayed AS char)
          , ' different from transaction amount = '
          , CAST(curTransactionAmount AS char)
          );
        END;
        /*
        -- check amount 
        ELSE
          IF curTransactionCommis <> inAmountCommis THEN
          BEGIN
            SET outUniKioskStatus = 25;
            SET outReturnVoucher = 1;
            SET exec_result = 0;
            SET error_msg = CONCAT(
            'inAmountCommis = '
            , CAST(inAmountCommis AS char)
            , ' different from transaction commis = '
            , CAST(curTransactionCommis AS char)
            );
          END;
          */
        -- check if voucher user for other service
        ELSE
          IF curVoucherIdComBill <> inIdComBill AND NOT ISNULL(curVoucherIdComBill) AND curFCVoucherStatus = 0 THEN
          BEGIN
            SET outUniKioskStatus = 22;
            SET outReturnVoucher = 1;
            SET exec_result = 0;
            SET error_msg = CONCAT('Already used voucher '
            , ' inIdComBill = '
            , CAST(inIdComBill AS char)
            , ' curVoucherIdComBill = '
            , CAST(curVoucherIdComBill AS char)
            );
          END;
          /*
            -- check givc bill status
            ELSE
              IF (curTransactionStatus <> 1 AND curTransactionStatus <> 2 AND curTransactionStatus <> 3)
              AND (inProviderAcceptStatus = 0 AND inMillAcceptStatus = 0)
              THEN
                BEGIN
                  SET outUniKioskStatus = 10;
                  SET outReturnVoucher = 0;
                  SET exec_result = 0;
                  SET error_msg = 'TransactionStatus not in 1,2,3';
                END;
              */
          -- check if all ok
          ELSE
            IF inProviderAcceptStatus = 0 AND inMillAcceptStatus = 0 THEN
            BEGIN
              SET outUniKioskStatus = 10;
              SET outReturnVoucher = 0;
              SET exec_result = 0;
              SET error_msg = '';
            END;
            -- check if error pay but we accept voucher
            ELSE
              IF inProviderAcceptStatus <> 0 AND inMillAcceptStatus = 0 THEN
              BEGIN
                -- if unknown client
                IF inUniKioskStatus = 15 THEN
                  SET outUniKioskStatus = 15;
                  SET outReturnVoucher = 1;
                  SET exec_result = 0;
                  SET error_msg = 'Unknown client';

                -- if absent check
                ELSE
                  IF inUniKioskStatus = 38 THEN
                    SET outUniKioskStatus = 38;
                    SET outReturnVoucher = 1;
                    SET exec_result = 0;
                    SET error_msg = 'Не прописан чек';

                  ELSE
                    IF curIsProviderOvertaking = 1 THEN
                      SET outUniKioskStatus = 12;
                      SET outSendToOvertaking = 1;
                      SET outReturnVoucher = 0;
                      SET exec_result = 0;
                      SET error_msg = CONCAT('Not success and send to overtaking '
                      , ' outUniKioskStatus = '
                      , CAST(outUniKioskStatus AS char)
                      );
                    ELSE
                      -- case if we dont return money
                      IF inUniKioskStatus IN (21, 23, 12, 13, 25, 100, 101) THEN
                        SET outUniKioskStatus = inUniKioskStatus;
                        SET outReturnVoucher = 0;
                        SET exec_result = 0;
                        SET error_msg = CONCAT('Not succes and dont return money'
                        , ' outUniKioskStatus = '
                        , CAST(outUniKioskStatus AS char)
                        );

                      -- return money always
                      ELSE
                        SET outUniKioskStatus = inUniKioskStatus;
                        SET outReturnVoucher = 1;
                        SET exec_result = 0;
                        SET error_msg = CONCAT('Not succes and return money'
                        , ' outUniKioskStatus = '
                        , CAST(outUniKioskStatus AS char)
                        );
                      END IF;
                    END IF;

                  /*
                      SET outReturnVoucher = 0;
                      SET exec_result = 0;
                      SET error_msg = CONCAT('Not success '
                      , ' outUniKioskStatus = '
                      , CAST(outUniKioskStatus AS char)
                      );
                      */
                  END IF;
                END IF;
              END;
              -- unknown error
              ELSE
              BEGIN
                SET outUniKioskStatus = 25;
                SET outReturnVoucher = 0;
                SET exec_result = 2450;
                SET error_msg = 'Unknown transaction status';
              END;

              END IF;
            END IF;
          END IF;
        END IF;
      END IF;
    END IF;
  END IF;
  --  END IF;

  -- if bill was already payed
  IF outUniKioskStatus = 11 THEN

    SELECT
      Payed INTO outPayDateTime
    FROM TransactionsComm
    WHERE IdComBill = inIdComBill;

    -- clear transaction lock
    UPDATE MillTx
    SET LockSt = 0
    WHERE ExtReqNo = inExtReqNo
    AND IdBuyer = curIdBuyer;

    LEAVE main;
  END IF;

  -- store data in MillTx
  IF ISNULL(curFCVoucherStatus) THEN

    IF inIdMillRq = - 1 THEN
      INSERT MillTx (IdComBill, IdBuyer, IdService, ExtReqNo, Amount, Commission, Accepted, MillOperationId, MillAccepted, MillRqStatus, LockSt)
        SELECT
          inIdComBill,
          curIdBuyer,
          inIdService,
          inExtReqNo,
          inAmountPayed,
          inAmountCommis,
          outPayDateTime,
          inMillOperationId,
          inMillAcceptTime,
          inMillAcceptStatus,
          1
      ;
    ELSE
      INSERT MillTx (IdMillRq, IdComBill, IdBuyer, IdService, ExtReqNo, Amount, Commission, Accepted, MillOperationId, MillAccepted, MillRqStatus, LockSt)
        SELECT
          inIdMillRq,
          inIdComBill,
          curIdBuyer,
          inIdService,
          inExtReqNo,
          inAmountPayed,
          inAmountCommis,
          outPayDateTime,
          inMillOperationId,
          inMillAcceptTime,
          inMillAcceptStatus,
          1
      ;
    END IF;

  ELSE

    /*
    UPDATE MillTx
    SET Amount = inAmountPayed + inAmountCommis,
        Accepted = outPayDateTime,
        MillOperationId = inMillOperationId,
        MillAccepted = inMillAcceptTime,
        MillRqStatus = inMillAcceptStatus,
        LockSt = 0
    WHERE ExtReqNo = inExtReqNo
    AND IdBuyer = curIdBuyer;
    */

    IF inMillAcceptStatus = 0 THEN

      UPDATE MillTx
      SET Amount = inAmountPayed,
          Commission = inAmountCommis,
          Accepted = outPayDateTime,
          MillOperationId = inMillOperationId,
          --          MillAccepted = inMillAcceptTime,
          MillRqStatus = inMillAcceptStatus,
          LockSt = 0
      WHERE ExtReqNo = inExtReqNo
      AND IdBuyer = curIdBuyer;

      IF inUpdateMillAcceptTime = 1 THEN
        UPDATE MillTx
        SET MillAccepted = inMillAcceptTime
        WHERE ExtReqNo = inExtReqNo
        AND IdBuyer = curIdBuyer;
      END IF;

    ELSE
      UPDATE MillTx
      SET Amount = inAmountPayed,
          Commission = inAmountCommis,
          --          Accepted = outPayDateTime,
          --          MillOperationId = inMillOperationId,
          --          MillAccepted = inMillAcceptTime,
          MillRqStatus = inMillAcceptStatus,
          LockSt = 0
      WHERE ExtReqNo = inExtReqNo
      AND IdBuyer = curIdBuyer;

    END IF;

  END IF;

  -- store data for table TransactionsComm
  IF (outUniKioskStatus = 10 OR outUniKioskStatus = 11) AND inMillAcceptStatus = 0 THEN
    SET newTransactionStatus = 0;
  -- if absent cheque return money
  ELSE
    IF outUniKioskStatus = 38 THEN
      SET newTransactionStatus = 24;
    ELSE
      SET newTransactionStatus = outUniKioskStatus;
    END IF;
  END IF;

  /*
  -- if string to long
  IF LENGTH(inKioskMessage) > 50 THEN
    SET inName = CONCAT(SUBSTRING(inKioskMessage, 1, 47), '...');
  ELSE
    SET inName = inKioskMessage;
  END IF;
    */

  -- calculate commission
  SET newCommissionAmt = inAmountCommis;
  --  newCommissionBank = inAmountPayed * curByuerCommRate;

  -- if new record
  IF ISNULL(curTransactionStatus) THEN
  BEGIN

    IF inIdTransaction = - 1 THEN
      INSERT TransactionsComm (IdComBill, IdBuyer, Service, /*IdSubService, */ IdClient, IdTerm, name, Amount, CommissionAmt, CommissionBank, IdCompany, Fulfilled, Payed, FinishDate, TransactionStatus, ParentId)
        SELECT
          inIdComBill,
          curIdBuyer,
          inIdService,
          /*inIdSubService,*/
          inIdClient,
          inIdTerm,
          inName,
          inAmountPayed,
          newCommissionAmt,
          0, --            newCommissionBank,
          inIdCompany,
          outPayDateTime,
          inMillAcceptTime,
          newFinishDate,
          newTransactionStatus,
          inProviderExtNo
      ;
    ELSE
      INSERT TransactionsComm (IdTransaction, IdComBill, IdBuyer, Service, /*IdSubService, */ IdClient, IdTerm, name, Amount, CommissionAmt, CommissionBank, IdCompany, Fulfilled, Payed, FinishDate, TransactionStatus, ParentId)
        SELECT
          inIdTransaction,
          inIdComBill,
          curIdBuyer,
          inIdService,
          /*inIdSubService,*/
          inIdClient,
          inIdTerm,
          inName,
          inAmountPayed,
          newCommissionAmt,
          0, --        newCommissionBank,
          inIdCompany,
          outPayDateTime,
          inMillAcceptTime,
          newFinishDate,
          newTransactionStatus,
          inProviderExtNo
      ;
    END IF;
  END;
  ELSE
  BEGIN
    UPDATE TransactionsComm
    SET IdBuyer = curIdBuyer,
        Service = inIdService,
        /*IdSubService = inIdSubService,*/
        IdClient = inIdClient,
        IdTerm = inIdTerm,
        name = IF(inName = '', name, SUBSTRING(TRIM(CONCAT(name, '|', inName)), 0, 80)), -- edited BS 28.09.2018
        Amount = inAmountPayed,
        CommissionAmt = NewCommissionAmt,
        CommissionBank = IFNULL(CommissionBank, 0), -- NewCommissionBank, -- Fulfilled = now(), -- edited BS 23.11.2018
        IdCompany = inIdCompany,
        TransactionStatus = newTransactionStatus,
        ParentId = inProviderExtNo,
        Fulfilled = outPayDateTime

    WHERE IdComBill = inIdComBill;

  END;
  END IF;

  IF inUpdateMillAcceptTime = 1 THEN
    UPDATE TransactionsComm
    SET Payed = inMillAcceptTime
    WHERE IdComBill = inIdComBill;
  END IF;

  -- if bill was already payed
  -- Store: D:\JOB\PROJECTS\VC10_Transition\PROJECTS\IntegrateUniInterface\Project\UniServiceAndHttp\UniService\DB\UniserviceBill\CommonProcess113Stage2_v7.sql
  IF outUniKioskStatus in (0,1,2,5,6,7,10,30,50,110,15,16,17,18,20,24,28,29,40,41,44,45) THEN
    BEGIN
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
    END;
  END IF;

  -- clear transaction lock
  UPDATE MillTx
  SET LockSt = 0
  WHERE ExtReqNo = inExtReqNo
  AND IdBuyer = curIdBuyer;

END $$

DELIMITER ;