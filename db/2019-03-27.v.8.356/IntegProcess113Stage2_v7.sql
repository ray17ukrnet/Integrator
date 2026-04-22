DELIMITER $$

DROP PROCEDURE IF EXISTS `tradedb3_1`.`IntegProcess113Stage2_v7` $$
CREATE PROCEDURE `tradedb3_1`.`IntegProcess113Stage2_v7` (IN inIdComBill bigint,
IN inIdMillRq bigint,
IN inIdTransaction bigint,
IN inExtReqNo varchar(40),
IN inIdService int,
IN inIdSubService int,
IN inIdClient varchar(128),
in inName  varchar(50),
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
IN inUniKioskStatus int
)
main:
BEGIN
  -- D:\JOB\PROJECTS\VC10_Transition\PROJECTS\IntegrateUniInterface\Project\UniServiceAndHttp\UniService\DB\UniserviceBill\IntegProcess113Stage2_v7.sql
  -- inProviderAcceptStatus
  --  pasUnknown = -1,									// неизвестный
  --  pasOK = 0,                        // счет успешно оплачен
  --  pasTimeout = 23,								  // Таймаут при обращении к провайдеру
  --  pasOutOfMoney = 17,						  	// закончились наши деньги у провайдера
  --  pasUnknownClient = 15,            // неизвестный клиент
  --  pasError = 24									    // постоянная ошибка провайдера

  -- outReturnVoucher 1 - return 0 - dont return

  -- result that returned from stored procedure
  DECLARE exec_result int;
  DECLARE error_msg char(80);

  DECLARE outUniKioskStatus int;
  DECLARE outPayDateTime datetime;
  DECLARE outReturnVoucher tinyint;
  DECLARE outSendToOvertaking tinyint;

  -- set value for returned result
  SET exec_result = 0;
  SET error_msg = '';
  SET outUniKioskStatus = 23;
  SET outPayDateTime = NOW();
  SET outReturnVoucher = 1;
  SET outSendToOvertaking = 0;

  -- Do common process
  CALL CommonProcess113Stage2_v7(
    inIdComBill,
    inIdMillRq,
    inIdTransaction,
    inExtReqNo,
    inIdService,
    inIdSubService,
    inIdClient,
    inName,
    inAmountPayed,
    inAmountCommis,
    inIdCompany,
    inIdTerm,
    inKeyValue,
    inPayDateTime,
    inMillOperationId,
    inMillAcceptTime,
    inMillAcceptStatus,
    inProviderAcceptStatus,
    inProviderExtNo,
    inUpdateMillAcceptTime,
    inUniKioskStatus,
    
    outUniKioskStatus,
    outPayDateTime,
    outReturnVoucher,
    outSendToOvertaking,
    
    exec_result,
    error_msg
      );

  SELECT
    exec_result,
    error_msg,
    outUniKioskStatus,
    outPayDateTime,
    outReturnVoucher,
    outSendToOvertaking
  ;


END $$

DELIMITER ;