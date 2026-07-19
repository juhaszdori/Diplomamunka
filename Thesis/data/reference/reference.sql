  SELECT 
         id_db       AS id,
         id_material AS product,
         b_default   AS "default"--,
		     --c_version   AS version
    FROM  
         recipetab 
   WHERE 
         d_dbend     = 99991231235959
     AND b_dbdeleted = false
     AND b_active    = true
     AND e_workingtype = 0
ORDER BY
         id_material;



  SELECT 
         id_db                             AS id,
         id_recipe                         AS recipe,
         i_order                           AS "order",
  	     id_operation                      AS "operation", 
  	     n_operationtime ::numeric / 10000 AS operationtime,
  	     e_operationtimeunit               AS operationtimeunit,
  	     n_basequantity  ::numeric / 10000 AS basequantity,
  	     id_basequantityunit               AS basequantityunit,
  	     --n_preparationtime  ::numeric / 10000 AS preparationtime,
  	     --n_changeovertime   ::numeric / 10000 AS changeovertime,
  	     --n_finalizationtime ::numeric / 10000 AS finalizationtime,
  	     n_runningscrap     ::numeric / 10000 AS runningscrap,
  	     e_productionmode                  AS productionmode
    FROM 
         recipeitemtab 
   WHERE 
         d_dbend     = 99991231235959
     AND b_dbdeleted = false
ORDER BY
         id_recipe, i_order;
   
   

  SELECT 
         id_db                             AS id,
         id_recipeitem                     AS recipeitem,
		     id_machine                        AS machine,
  	     n_operationtime ::numeric / 10000 AS operationtime,
  	     n_basequantity  ::numeric / 10000 AS basequantity,
  	     id_basequantityunit               AS basequantityunit
    FROM
         machinedemandtab
   WHERE 
         d_dbend     = 99991231235959
     AND b_dbdeleted = false
ORDER BY 
         id_recipeitem, i_order;



  SELECT 
         materialdemandgrouptab.id                          AS id,
         materialdemandgrouptab.id_recipeitem               AS recipeitem,
		     --materialdemandgrouptab.i_order                     AS "order",
		     --materialdemandgrouptab.n_ratio ::numeric / 10000   AS ratio,
		     materialdemandtab.id_material                      AS material,
		     materialdemandtab.e_type                           AS "type",
		     materialdemandtab.n_piece        ::numeric / 10000 AS piece,
		     materialdemandtab.n_basequantity ::numeric / 10000 AS basequantity,
		     materialdemandtab.id_basequantityunit              AS basequantityunit --,
		     --b_mainrawmaterial                                  AS mainrawmaterial
		     --b_alternativeallowed                               AS alternativeallowed,
		     --b_usedforspecificmachinesonly                      AS usedforspecificmachinesonly
    FROM 
	       materialdemandtab
    JOIN 
	       materialdemandgrouptab
      ON materialdemandgrouptab.id_db       = materialdemandtab.id_materialdemandgroup
   WHERE 
         materialdemandgrouptab.d_dbend     = 99991231235959
     AND materialdemandtab.d_dbend          = 99991231235959
     AND materialdemandgrouptab.b_dbdeleted = false
     AND materialdemandtab.b_dbdeleted      = false
ORDER BY 
         materialdemandgrouptab.id_recipeitem,
		     materialdemandgrouptab.i_order,
		     materialdemandtab.i_order;


