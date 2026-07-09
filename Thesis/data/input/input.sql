-- gyártási események: 
 
  SELECT
         tasktab.id_material                                 AS product,
		     tasktab.id_db                                       AS task,
         --jobtab.id_db                                        AS job,
         productionrecipeitemtab.id_operation                AS "operation",
         productionreportitemtab.n_quantity::numeric / 10000 AS quantity,
         materialtab.id_quantityunit                         AS quantityunit,
         productionreportitemtab.id_material                 AS material,
         productionreportitemtab.e_type                      AS eventtype,
         productionreporttab.id_machine                      AS machine,
         productionreporttab.d_creationdate                  AS "timestamp"--,
         --productionreporttab.d_date                          AS "date"
    FROM
         productionreportitemtab
    JOIN 
         productionreporttab
      ON productionreporttab.id_db            = productionreportitemtab.id_productionreport
     AND productionreporttab.d_dbend          = 99991231235959
	   AND productionreporttab.b_dbdeleted      = false
     AND productionreporttab.e_receipttype    = 0
     AND productionreporttab.d_creationdate   > 20260301000000
     AND productionreporttab.e_reportingtype IN (0,1)
     --AND productionreporttab.b_manuallycreated = false
    JOIN 
         jobtab
      ON jobtab.id_db       = productionreporttab.id_job
     AND jobtab.e_status   >= 5
     AND jobtab.d_dbend     = 99991231235959
     AND jobtab.b_dbdeleted = false
    JOIN 
         tasktab 
      ON tasktab.id_db         = jobtab.id_task
     AND tasktab.b_dbdeleted   = false
     AND tasktab.d_dbend       = 99991231235959
     AND tasktab.e_workingtype = 0
     --AND tasktab.b_reportedbyhand = false
    JOIN 
         productionrecipeitemtab
      ON productionrecipeitemtab.id_job      = jobtab.id_db
     AND productionrecipeitemtab.d_dbend     = 99991231235959
     AND productionrecipeitemtab.b_dbdeleted = false
    JOIN 
         materialtab
      ON materialtab.id_db       = productionreportitemtab.id_material
     AND materialtab.d_dbend     = 99991231235959
     AND materialtab.b_dbdeleted = false
  
   WHERE 
         productionreportitemtab.d_dbend      = 99991231235959
     AND productionreportitemtab.b_dbdeleted  = false
     AND productionreportitemtab.e_type      IN (0,1,4)
ORDER BY 
         product, task, productionrecipeitemtab.id_operation, productionreporttab.d_creationdate;
         
-- idők:

  SELECT 
         tasktab.id_material                  AS product,
         tasktab.id_db                        AS task,                  
         productionrecipeitemtab.id_operation AS "operation",                                 
         jobstatusrecordtab.id_machine        AS machine,                     
         jobstatusrecordtab.d_beg             AS timebeg,                     
         jobstatusrecordtab.d_end             AS timeend                   
    FROM                                                                                             
         jobstatusrecordtab
	  JOIN 
         jobtab
      ON jobtab.id_db                        = jobstatusrecordtab.id_job
     AND jobtab.d_dbend                      = 99991231235959
     AND jobtab.b_dbdeleted                  = false
	   AND jobtab.e_status                    >= 5
	   AND jobtab.d_realbeg                    > 20260301000000
	JOIN 
         tasktab 
      ON tasktab.id_db                       = jobtab.id_task
     AND tasktab.b_dbdeleted                 = false
     AND tasktab.d_dbend                     = 99991231235959
     AND tasktab.e_workingtype               = 0
	JOIN 
         productionrecipeitemtab
      ON productionrecipeitemtab.id_job      = jobtab.id_db
     AND productionrecipeitemtab.d_dbend     = 99991231235959
     AND productionrecipeitemtab.b_dbdeleted = false
   WHERE
         jobstatusrecordtab.b_dbdeleted      = false 
     AND jobstatusrecordtab.d_dbend          = 99991231235959 
     AND jobstatusrecordtab.e_status         = 2 
ORDER BY 
         tasktab.id_db,
         productionrecipeitemtab.id_operation,
         jobstatusrecordtab.id_machine,
		     jobstatusrecordtab.d_beg;