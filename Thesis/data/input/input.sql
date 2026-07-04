
select * from materialdemandtab where materialdemandtab.d_dbend = 99991231235959
and materialdemandtab.b_dbdeleted = false;

select id_material,
e_type,
n_basequantity,
n_basequantityunit

from materialdemandtab where materialdemandtab.d_dbend = 99991231235959
and materialdemandtab.b_dbdeleted = false;

select productionrecipeitemtab.id_operation
from productionrecipeitemtab where productionrecipeitemtab.d_dbend = 99991231235959
and productionrecipeitemtab.b_dbdeleted = false;

select * from productionreporttab 
where d_dbend = 99991231235959
and b_dbdeleted = false;

select * from operationtab 
where operationtab.d_dbend = 99991231235959
and operationtab.b_dbdeleted = false;

select *
from tasktab where tasktab.d_dbend = 99991231235959
and tasktab.b_dbdeleted = false
and tasktab.e_workingtype = 0;

select 
jobtab.d_realbeg,
jobtab.d_realend
from jobtab where jobtab.d_dbend = 99991231235959
and jobtab.b_dbdeleted = false;

select
productionreporttab.id_job,
productionreporttab.id_machine
from productionreporttab
where productionreporttab.d_dbend = 99991231235959
and productionreporttab.d_creationdate > 20260301000000
and productionreporttab.d_creationdate < 20260401000000
and productionreporttab.b_dbdeleted = false
and (productionreporttab.e_reportingtype = 0 or productionreporttab.e_reportingtype = 1);

select * from productionreportitemtab
where productionreportitemtab.d_dbend = 99991231235959
and productionreportitemtab.b_dbdeleted = false
and (productionreportitemtab.e_type = 0 or productionreportitemtab.e_type = 1);


SELECT tasktab.id_db AS task_id,
productionrecipeitemtab.id_operation AS operation,
productionreportitemtab.n_quantity AS quantity,
materialtab.id_quantityunit AS quantityunit,
productionreportitemtab.id_material AS material,
productionreportitemtab.e_type AS material_type,
productionreporttab.id_machine AS machine,
jobtab.d_realbeg AS start_time,
jobtab.d_realend AS end_time,
productionreportitemtab.id_db AS productionreportitem_id,
productionreporttab.id_db AS productionreport_id
FROM productionreportitemtab
JOIN productionreporttab ON productionreportitemtab.id_productionreport = productionreporttab.id_db
JOIN jobtab ON productionreporttab.id_job = jobtab.id_db
JOIN tasktab ON jobtab.id_task = tasktab.id_db
JOIN productionrecipeitemtab ON productionrecipeitemtab.id_job = jobtab.id_db
JOIN materialtab ON productionreportitemtab.id_material = materialtab.id_db
JOIN operationtab ON productionrecipeitemtab.id_operation = operationtab.id_db
WHERE tasktab.d_dbend = 99991231235959
AND tasktab.b_dbdeleted = false
AND tasktab.e_workingtype = 0
AND jobtab.e_status >= 5
AND jobtab.d_dbend = 99991231235959
AND jobtab.b_dbdeleted = false
AND productionreporttab.d_dbend = 99991231235959
AND productionreporttab.e_receipttype = 0
AND productionreporttab.d_creationdate > 20260301000000

AND productionreporttab.b_dbdeleted = false
AND (productionreporttab.e_reportingtype = 0 or productionreporttab.e_reportingtype = 1)
AND productionreportitemtab.d_dbend = 99991231235959
AND productionreportitemtab.b_dbdeleted = false
AND productionreportitemtab.b_dbarchive = false
AND (productionreportitemtab.e_type = 0 or productionreportitemtab.e_type = 1)
ORDER BY tasktab.id_db, operationtab.id_db, productionreporttab.d_creationdate;




SELECT
  tasktab.id_db                              AS task,
  jobtab.id_db                               AS job,
  tasktab.id_material                        AS product,
  productionrecipeitemtab.id_operation       AS operation,
  productionreportitemtab.n_quantity / 10000 AS quantity,
  materialtab.id_quantityunit                AS quantityunit,
  productionreportitemtab.id_material        AS material,
  productionreportitemtab.e_type             AS event_type,
  productionreporttab.id_machine             AS machine,
  productionreporttab.d_creationdate         AS dt_timestamp
  jobtab.d_realbeg                           AS job_real_beg
  --jobstatusrecordtab.d_beg                     AS status_beg,
  --jobstatusrecordtab.d_end                     AS status_end
  --jobtab.id_db                               AS job
FROM productionreportitemtab
JOIN productionreporttab
   ON productionreportitemtab.id_productionreport = productionreporttab.id_db
  AND productionreporttab.d_dbend = 99991231235959
  AND productionreporttab.e_receipttype = 0
  AND productionreporttab.d_creationdate > 20260301000000
  AND productionreporttab.b_dbdeleted = false
  AND productionreporttab.e_reportingtype IN (0,1)
JOIN jobtab
   ON productionreporttab.id_job = jobtab.id_db
  AND jobtab.e_status >= 5
  AND jobtab.d_dbend = 99991231235959
  AND jobtab.b_dbdeleted = false
JOIN tasktab 
   ON jobtab.id_task = tasktab.id_db
  AND tasktab.b_dbdeleted = false
  AND tasktab.d_dbend = 99991231235959
  AND tasktab.e_workingtype = 0
JOIN productionrecipeitemtab
   ON productionrecipeitemtab.id_job = jobtab.id_db
  AND productionrecipeitemtab.d_dbend = 99991231235959
  AND productionrecipeitemtab.b_dbdeleted = false
JOIN materialtab
   ON productionreportitemtab.id_material = materialtab.id_db
  AND materialtab.d_dbend = 99991231235959
  AND materialtab.b_dbdeleted = false
--LEFT JOIN jobstatusrecordtab
--   ON jobstatusrecordtab.id_job = jobtab.id_db
--  AND jobstatusrecordtab.id_machine = productionreporttab.id_machine
--  AND jobstatusrecordtab.e_status = 2
  --AND jobstatusrecordtab.d_beg < productionreporttab.d_creationdate
  --AND jobstatusrecordtab.d_end >= productionreporttab.d_creationdate
--  AND jobstatusrecordtab.b_dbdeleted = false
--  AND jobstatusrecordtab.d_dbend = 99991231235959
WHERE productionreportitemtab.d_dbend = 99991231235959
  AND productionreportitemtab.b_dbdeleted = false
  AND productionreportitemtab.e_type IN (0,1,4)
ORDER BY task;

