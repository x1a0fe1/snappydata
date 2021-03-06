INCLUDE $JTESTS/hydraconfig/hydraparams1.inc;
INCLUDE $JTESTS/hydraconfig/topology_3.inc;

hydra.gemfirexd.GfxdHelperPrms-persistDD = true;
hydra.gemfirexd.GfxdHelperPrms-createDiskStore = true;
hydra.GemFirePrms-names = gemfire1;
hydra.ClientPrms-gemfireNames = gemfire1;
hydra.GemFirePrms-distributedSystem = ds;

THREADGROUP snappyStoreThreads
            totalThreads = fcn "(${${A}Hosts} * ${${A}VMsPerHost} *  ${${A}ThreadsPerVM}) " ncf
            totalVMs     = fcn "(${${A}Hosts} * ${${A}VMsPerHost})" ncf
            clientNames  = fcn "hydra.TestConfigFcns.generateNames(\"${A}\",
                                ${${A}Hosts}, true)" ncf;

THREADGROUP leadThreads
            totalThreads = fcn "(${${B}Hosts} * ${${B}VMsPerHost} *  ${${B}ThreadsPerVM}) -1 " ncf
            totalVMs     = fcn "(${${B}Hosts} * ${${B}VMsPerHost})" ncf
            clientNames  = fcn "hydra.TestConfigFcns.generateNames(\"${B}\",
                                ${${B}Hosts}, true)" ncf;

THREADGROUP locatorThreads
            totalThreads = fcn "(${${C}Hosts} * ${${C}VMsPerHost} *  ${${C}ThreadsPerVM}) " ncf
            totalVMs     = fcn "(${${C}Hosts} * ${${C}VMsPerHost})" ncf
            clientNames  = fcn "hydra.TestConfigFcns.generateNames(\"${C}\",
                                ${${C}Hosts}, true)" ncf;

THREADGROUP snappyThreads
            totalThreads = 1
            totalVMs     = 1
            clientNames  = fcn "hydra.TestConfigFcns.generateNames(\"${B}\",
                                ${${B}Hosts}, true)" ncf;

INITTASK   taskClass   = io.snappydata.hydra.cluster.SnappyTest taskMethod  = HydraTask_initializeSnappyTest
           runMode = always
           threadGroups = snappyThreads, locatorThreads, snappyStoreThreads, leadThreads;

INITTASK   taskClass   = io.snappydata.hydra.cluster.SnappyTest taskMethod  = HydraTask_generateSnappyLocatorConfig
           runMode = always
           threadGroups = locatorThreads;

INITTASK   taskClass   = io.snappydata.hydra.cluster.SnappyTest taskMethod  = HydraTask_writeLocatorConfigData
           runMode = always
           threadGroups = snappyThreads;

INITTASK   taskClass   = io.snappydata.hydra.cluster.SnappyTest taskMethod  = HydraTask_generateSnappyServerConfig
           runMode = always
           threadGroups = snappyStoreThreads;

INITTASK   taskClass   = io.snappydata.hydra.cluster.SnappyTest taskMethod  = HydraTask_writeServerConfigData
           runMode = always
           threadGroups = snappyThreads;

INITTASK   taskClass   = io.snappydata.hydra.cluster.SnappyTest taskMethod  = HydraTask_generateSnappyLeadConfig
           runMode = always
           threadGroups = leadThreads, snappyThreads;

INITTASK   taskClass   = io.snappydata.hydra.cluster.SnappyTest taskMethod  = HydraTask_writeLeadConfigData
           runMode = always
           threadGroups = snappyThreads;

INITTASK   taskClass   = io.snappydata.hydra.cluster.SnappyTest taskMethod  = HydraTask_createAndStartSnappyLocator
           runMode = always
           threadGroups = locatorThreads;

INITTASK   taskClass   = io.snappydata.hydra.cluster.SnappyTest taskMethod  = HydraTask_createAndStartSnappyServers
           runMode = always
           threadGroups = snappyStoreThreads;

INITTASK   taskClass   = io.snappydata.hydra.cluster.SnappyTest taskMethod  = HydraTask_createAndStartSnappyLeader
           runMode = always
           threadGroups = leadThreads;

INITTASK   taskClass   = io.snappydata.hydra.cluster.SnappyTest taskMethod  = HydraTask_getClientConnection
           threadGroups = snappyThreads;

INITTASK   taskClass   = io.snappydata.hydra.cluster.SnappyTest taskMethod  = HydraTask_executeSQLScripts
           io.snappydata.hydra.cluster.SnappyPrms-sqlScriptNames = create_and_load_column_table.sql create_and_load_sample_table.sql create_and_load_row_table.sql olap_queries.sql
           io.snappydata.hydra.cluster.SnappyPrms-dataLocation = airlineParquetData " " airportcodeParquetData
           threadGroups = snappyThreads
           ;

INITTASK   taskClass   = io.snappydata.hydra.cluster.SnappyTest taskMethod  = runQuery
           threadGroups = snappyThreads;

TASK       taskClass   = io.snappydata.hydra.cluster.SnappyTest taskMethod  = HydraTask_executeSQLScripts
           io.snappydata.hydra.cluster.SnappyPrms-sqlScriptNames = olap_queries.sql oltp_queries.sql
           threadGroups = snappyThreads
           ;

TASK       taskClass   = io.snappydata.hydra.cluster.SnappyTest taskMethod  = HydraTask_executeSnappyJob
           io.snappydata.hydra.cluster.SnappyPrms-jobClassNames = io.snappydata.hydra.AirlineDataQueriesJob
           io.snappydata.hydra.cluster.SnappyPrms-userAppJar = snappydata-store-scala-tests*tests.jar
           io.snappydata.hydra.cluster.SnappyPrms-numTimesToRetry = 10
           threadGroups = leadThreads
           maxTimesToRun = 3;

ENDTASK    taskClass   = io.snappydata.hydra.cluster.SnappyTest taskMethod  = HydraTask_cleanUpSnappyProcessesOnFailure
           clientNames = locator1;

hydra.Prms-totalTaskTimeSec           = 900;
hydra.Prms-maxResultWaitSec           = 1800;

hydra.Prms-maxCloseTaskResultWaitSec  = 1800;
hydra.Prms-serialExecution            = false;

hydra.CachePrms-names = defaultCache;
sql.SQLPrms-useGfxdConfig = true;

/* end task must stop snappy members because they are not stopped by Hydra */
hydra.Prms-alwaysDoEndTasks = true;

hydra.VmPrms-extraVMArgs   += fcn "hydra.TestConfigFcns.duplicate
                                  (\"-Xms512m -Xmx1g \", ${${A}Hosts}, true)"
                             ncf
                             ,
                             fcn "hydra.TestConfigFcns.duplicate
                                  (\"-Xms512m -Xmx1g \", ${${B}Hosts}, true)"
                             ncf;
