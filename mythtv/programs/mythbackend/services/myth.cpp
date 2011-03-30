//////////////////////////////////////////////////////////////////////////////
// Program Name: myth.cpp
// Created     : Jan. 19, 2010
//
// Copyright (c) 2010 David Blain <dblain@mythtv.org>
//                                          
// This library is free software; you can redistribute it and/or 
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or at your option any later version of the LGPL.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library.  If not, see <http://www.gnu.org/licenses/>.
//
//////////////////////////////////////////////////////////////////////////////

#include "myth.h"

#include <QDir>
#include <QCryptographicHash>

#include "mythcorecontext.h"
#include "mythdbcon.h"
#include "storagegroup.h"

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

DTC::ConnectionInfo* Myth::GetConnectionInfo( const QString  &sPin )
{
    QString sSecurityPin = gCoreContext->GetSetting( "SecurityPin", "");

    if ( sSecurityPin.isEmpty() )
        throw( QString( "No Security Pin assigned. Run mythtv-setup to set one." ));
        //SB: UPnPResult_HumanInterventionRequired,

    if ((sSecurityPin != "0000" ) && ( sPin != sSecurityPin ))
        throw( QString( "Not Authorized" ));
        //SB: UPnPResult_ActionNotAuthorized );
  
    DatabaseParams params = gCoreContext->GetDatabaseParams();

    // ----------------------------------------------------------------------
    // Check for DBHostName of "localhost" and change to public name or IP
    // ----------------------------------------------------------------------

    QString sServerIP = gCoreContext->GetSetting( "BackendServerIP", "localhost" );
    //QString sPeerIP   = pRequest->GetPeerAddress();

    if ((params.dbHostName == "localhost") &&
        (sServerIP         != "localhost"))  // &&
        //(sServerIP         != sPeerIP    ))
    {
        params.dbHostName = sServerIP;
    }

    // ----------------------------------------------------------------------
    // Create and populate a ConnectionInfo object
    // ----------------------------------------------------------------------

    DTC::ConnectionInfo *pInfo     = new DTC::ConnectionInfo();
    DTC::DatabaseInfo   *pDatabase = pInfo->Database();
    DTC::WOLInfo        *pWOL      = pInfo->WOL();

    pDatabase->setHost         ( params.dbHostName    );
    pDatabase->setPing         ( params.dbHostPing    );
    pDatabase->setPort         ( params.dbPort        );
    pDatabase->setUserName     ( params.dbUserName    );
    pDatabase->setPassword     ( params.dbPassword    );
    pDatabase->setName         ( params.dbName        );
    pDatabase->setType         ( params.dbType        );
    pDatabase->setLocalEnabled ( params.localEnabled  );
    pDatabase->setLocalHostName( params.localHostName );

    pWOL->setEnabled           ( params.wolEnabled   );
    pWOL->setReconnect         ( params.wolReconnect );
    pWOL->setRetry             ( params.wolRetry     );
    pWOL->setCommand           ( params.wolCommand   );

    // ----------------------------------------------------------------------
    // Return the pointer... caller is responsible to delete it!!!
    // ----------------------------------------------------------------------

    return pInfo;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

QString Myth::GetHostName( )
{
    if (!gCoreContext)
        throw( QString( "No MythCoreContext in GetHostName." ));

    return gCoreContext->GetHostName();
}
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

QStringList Myth::GetHosts( )
{
    MSqlQuery query(MSqlQuery::InitCon());

    if (!query.isConnected())
        throw( QString( "Database not open while trying to load list of hosts" ));

    query.prepare(
        "SELECT DISTINCTROW hostname "
        "FROM settings "
        "WHERE (not isNull( hostname ))");

    if (!query.exec())
    {
        MythDB::DBError("MythAPI::GetHosts()", query);

        throw( QString( "Database Error executing query." ));
    }

    // ----------------------------------------------------------------------
    // return the results of the query
    // ----------------------------------------------------------------------

    QStringList oList;

    while (query.next())
        oList.append( query.value(0).toString() );

    return oList;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

QStringList Myth::GetKeys() 
{
    MSqlQuery query(MSqlQuery::InitCon());

    if (!query.isConnected())
        throw( QString("Database not open while trying to load settings"));

    query.prepare("SELECT DISTINCTROW value FROM settings;" );

    if (!query.exec())
    {
        MythDB::DBError("MythAPI::GetKeys()", query);

        throw( QString( "Database Error executing query." ));
    }

    // ----------------------------------------------------------------------
    // return the results of the query
    // ----------------------------------------------------------------------

    QStringList oResults;
    
    //pResults->setObjectName( "KeyList" );

    while (query.next())
        oResults.append( query.value(0).toString() );

    return oResults;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

DTC::StorageGroupDirList *Myth::GetStorageGroupDirs( const QString &sGroupName,
                                                     const QString &sHostName ) 
{
    MSqlQuery query(MSqlQuery::InitCon());

    if (!query.isConnected())
        throw( QString("Database not open while trying to list "
                       "Storage Group Dirs"));

    if (!sGroupName.isEmpty() && !sHostName.isEmpty())
    {
        query.prepare("SELECT id, groupname, hostname, dirname "
                      "FROM storagegroup "
                      "WHERE groupname = :GROUP AND hostname = :HOST "
                      "ORDER BY groupname, hostname, dirname" );
        query.bindValue(":HOST",  sHostName);
        query.bindValue(":GROUP", sGroupName);
    }
    else if (!sHostName.isEmpty())
    {
        query.prepare("SELECT id, groupname, hostname, dirname "
                      "FROM storagegroup "
                      "WHERE hostname = :HOST "
                      "ORDER BY groupname, hostname, dirname" );
        query.bindValue(":HOST",  sHostName);
    }
    else if (!sGroupName.isEmpty())
    {
        query.prepare("SELECT id, groupname, hostname, dirname "
                      "FROM storagegroup "
                      "WHERE groupname = :GROUP "
                      "ORDER BY groupname, hostname, dirname" );
        query.bindValue(":GROUP", sGroupName);
    }
    else
        query.prepare("SELECT id, groupname, hostname, dirname "
                      "FROM storagegroup "
                      "ORDER BY groupname, hostname, dirname" );

    if (!query.exec())
    {
        MythDB::DBError("MythAPI::GetStorageGroupDirs()", query);

        throw( QString( "Database Error executing query." ));
    }

    // ----------------------------------------------------------------------
    // return the results of the query
    // ----------------------------------------------------------------------

    DTC::StorageGroupDirList* pList = new DTC::StorageGroupDirList();

    while (query.next())
    {
        DTC::StorageGroupDir *pStorageGroupDir = pList->AddNewStorageGroupDir();
            
        pStorageGroupDir->setId            ( query.value(0).toInt()       );
        pStorageGroupDir->setGroupName     ( query.value(1).toString()    );
        pStorageGroupDir->setHostName      ( query.value(2).toString()    );
        pStorageGroupDir->setDirName       ( query.value(3).toString()    );
    }

    return pList;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

bool Myth::AddStorageGroupDir( const QString &sGroupName,
                               const QString &sDirName, 
                               const QString &sHostName )
{
    MSqlQuery query(MSqlQuery::InitCon());

    if (!query.isConnected())
        throw( QString("Database not open while trying to add Storage Group "
                       "dir"));

    if (sGroupName.isEmpty())
        throw ( QString( "Storage Group Required" ));

    if (sDirName.isEmpty())
        throw ( QString( "Directory Name Required" ));

    if (sHostName.isEmpty())
        throw ( QString( "HostName Required" ));

    query.prepare("SELECT COUNT(*) "
                  "FROM storagegroup "
                  "WHERE groupname = :GROUPNAME "
                  "AND dirname = :DIRNAME "
                  "AND hostname = :HOSTNAME;");
    query.bindValue(":GROUPNAME", sGroupName );
    query.bindValue(":DIRNAME"  , sDirName   );
    query.bindValue(":HOSTNAME" , sHostName  );
    if (!query.exec())
    {
        MythDB::DBError("MythAPI::AddStorageGroupDir()", query);

        throw( QString( "Database Error executing query." ));
    }

    if (query.next())
    {
        if (query.value(0).toInt() > 0)
            return false;
    }

    query.prepare("INSERT storagegroup "
                  "( groupname, dirname, hostname ) "
                  "VALUES "
                  "( :GROUPNAME, :DIRNAME, :HOSTNAME );");
    query.bindValue(":GROUPNAME", sGroupName );
    query.bindValue(":DIRNAME"  , sDirName   );
    query.bindValue(":HOSTNAME" , sHostName  );

    if (!query.exec())
    {
        MythDB::DBError("MythAPI::AddStorageGroupDir()", query);

        throw( QString( "Database Error executing query." ));
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

bool Myth::RemoveStorageGroupDir( const QString &sGroupName,
                                  const QString &sDirName, 
                                  const QString &sHostName )
{
    MSqlQuery query(MSqlQuery::InitCon());

    if (!query.isConnected())
        throw( QString("Database not open while trying to remove Storage "
                       "Group dir"));

    if (sGroupName.isEmpty())
        throw ( QString( "Storage Group Required" ));

    if (sDirName.isEmpty())
        throw ( QString( "Directory Name Required" ));

    if (sHostName.isEmpty())
        throw ( QString( "HostName Required" ));

    query.prepare("DELETE "
                  "FROM storagegroup "
                  "WHERE groupname = :GROUPNAME "
                  "AND dirname = :DIRNAME "
                  "AND hostname = :HOSTNAME;");
    query.bindValue(":GROUPNAME", sGroupName );
    query.bindValue(":DIRNAME"  , sDirName   );
    query.bindValue(":HOSTNAME" , sHostName  );
    if (!query.exec())
    {
        MythDB::DBError("MythAPI::AddStorageGroupDir()", query);

        throw( QString( "Database Error executing query." ));
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

DTC::SettingList *Myth::GetSetting( const QString &sHostName, 
                                    const QString &sKey, 
                                    const QString &sDefault )
{

    MSqlQuery query(MSqlQuery::InitCon());

    if (!query.isConnected())
    {
        throw( QString("Database not open while trying to load setting: %1")
                  .arg( sKey ));
    }

    // ----------------------------------------------------------------------

    DTC::SettingList *pList = new DTC::SettingList();

    //pList->setObjectName( "Settings" );
    pList->setHostName  ( sHostName  );

    // ----------------------------------------------------------------------
    // Format Results
    // ----------------------------------------------------------------------

    if (!sKey.isEmpty())
    {
        // ------------------------------------------------------------------
        // Key Supplied, lookup just its value.
        // ------------------------------------------------------------------

        query.prepare("SELECT data, hostname from settings "
                      "WHERE value = :KEY AND "
                      "(hostname = :HOSTNAME OR hostname IS NULL) "
                      "ORDER BY hostname DESC;" );

        query.bindValue(":KEY"     , sKey      );
        query.bindValue(":HOSTNAME", sHostName );

        if (!query.exec())
        {
            // clean up unused object we created.

            delete pList;

            MythDB::DBError("MythAPI::GetSetting() w/key ", query);

            throw( QString( "Database Error executing query." ));
        }

        if (query.next())
        {
            if ( (sHostName.isEmpty()) ||
                 ((!sHostName.isEmpty()) &&
                  (sHostName == query.value(1).toString())))
            {
                pList->setHostName( query.value(1).toString() );

                pList->Settings().insert( sKey, query.value(0) );
            }
        }
    }
    else
    {
        // ------------------------------------------------------------------
        // Looking to return all Setting for supplied hostname 
        // ------------------------------------------------------------------

        if (sHostName.isEmpty())
        {
            query.prepare("SELECT value, data FROM settings "
                          "WHERE (hostname IS NULL)" );
        }
        else
        {
            query.prepare("SELECT value, data FROM settings "
                          "WHERE (hostname = :HOSTNAME)" );

            query.bindValue(":HOSTNAME", sHostName );
        }

        if (!query.exec())
        {
            // clean up unused object we created.

            delete pList;

            MythDB::DBError("MythAPI::GetSetting() w/o key ", query);
            throw( QString( "Database Error executing query." ));
        }

        while (query.next())
            pList->Settings().insert( query.value(0).toString(), query.value(1) );
    }

    // ----------------------------------------------------------------------
    // Not found, so return the supplied default value
    // ----------------------------------------------------------------------

    if (pList->Settings().count() == 0)
        pList->Settings().insert( sKey, sDefault );

    return pList;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

bool Myth::PutSetting( const QString &sHostName, 
                       const QString &sKey, 
                       const QString &sValue ) 
{
    bool bResult = false;

    if (!sKey.isEmpty())
    {
        if ( gCoreContext->SaveSettingOnHost( sKey, sValue, sHostName ) )
            bResult = true;

        return bResult;
    }

    throw ( QString( "Key Required" ));
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

bool Myth::ChangePassword( const QString   &sUserName,
                           const QString   &sOldPassword,
                           const QString   &sNewPassword )
{
    bool bResult = false;

    if (sUserName.isEmpty())
    {
        throw ( QString( "UserName not supplied when trying to change "
                         "password." ) );
    }

    if (sOldPassword.isEmpty())
    {
        throw ( QString( "Old Password not supplied when trying to change "
                         "password for '%1'." ).arg(sUserName) );
    }

    if (sNewPassword.isEmpty())
    {
        throw ( QString( "New Password not supplied when trying to change "
                         "password for '%1'." ).arg(sUserName) );
    }

    QCryptographicHash crypto( QCryptographicHash::Sha1 );

    crypto.addData( sOldPassword.toUtf8() );

    QString sPasswordHash( crypto.result().toBase64() );

    if ( sPasswordHash != gCoreContext->GetSetting( "HTTP/Protected/Password", ""))
    {
        throw ( QString( "Incorrect Old Password supplied when trying to "
                         "change password for '%1'." ).arg(sUserName) );
    }

    crypto.reset();
    crypto.addData( sNewPassword.toUtf8() );

    if (gCoreContext->SaveSettingOnHost( "HTTP/Protected/Password", crypto.result().toBase64(),
                                    QString() ) )
    {
        gCoreContext->ClearSettingsCache();
        bResult = true;
    }

    return bResult;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

bool Myth::TestDBSettings( const QString &sHostName,
                           const QString &sUserName,
                           const QString &sPassword,
                           const QString &sDBName,
                           int   dbPort)
{
    bool bResult = false;

    QString db("mythconverg");
    int port = 3306;

    if (!sDBName.isEmpty())
        db = sDBName;

    if (dbPort != 0)
        port = dbPort;

    bResult = TestDatabase(sHostName, sUserName, sPassword, db, port);

    return bResult;
}

