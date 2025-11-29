const net = require('net'), dotenv = require('dotenv'), mysql = require('mysql');

dotenv.config();

const mysqlconn = mysql.createPool({
    host: process.env.MYSQL_HOST
    , port: process.env.MYSQL_PORT
    , user: process.env.MYSQL_USER
    , password: process.env.MYSQL_PASS
    , database: process.env.MYSQL_DB
});

let server = net.createServer(function(socket) {

    socket.on('error', function(error) {
        console.error("Error received on socket (probably disconnect). Ignoring...");
    });

    socket.on('data', function(data) {
		console.log("[D] Received datapacket: " + data.toString());
        let parsedData = data.toString().replace(/\0/g, '').split("\\");
        let action = parsedData[0];

        switch (action) {
            case "heartbeat":
                socket.write('heartbeat\\ack');
                break;
            default:
                runSQL(action, parsedData, function(sqlOutput) {
                    

                    if (action === "adminLogin") {
                        if (!sqlOutput.error && sqlOutput.msg.length === 1) {
							console.log("[D] Sending response grantAdmin\t" + sqlOutput.msg[0].adminlevel);
                            socket.write("grantAdmin\\" + sqlOutput.msg[0].adminlevel + "\\" + sqlOutput.msg[0].user_id);
                        } else {
							console.log("[D] Sending response grantAdmin\t0");
                            socket.write("grantAdmin\\0\\-1");
                        }
                    } else {
						if (sqlOutput.error) {
							console.log("[D] Sending response unknowncommand");
							socket.write('unknowncommand');
						}
                       if (action === "serverStats" || action === "playerStats") {
                           socket.write("ack");
                       }

                   }
                
                });
                break;
        }
    });
});

function runSQL(action, parsedData, callback) {
    
    mysqlconn.getConnection(function(error, conn) {
        const actionQueries = {
            'logLogin' : 'INSERT INTO log_login (serveridentifier, byname, byip, adminlvl) VALUES (?, ?, ?, ?)'
            , 'logAdmin' : 'INSERT INTO log_admin (serveridentifier, byname, byip, toname, toip, admintext, adminaction, adminlevel, admname, admtype) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)'
            , 'logGame' : 'INSERT INTO log_game (serveridentifier, byname, byip, toname, toip, gametext, action) VALUES (?, ?, ?, ?, ?, ?, ?)'
            , 'logRcon' : 'INSERT INTO log_rcon (serveridentifier, byip, action, message) VALUES (?, ?, ?, ?)'
            , 'logDamage' : 'INSERT INTO log_damage (serveridentifier, mapname, byip, byname, toname, toip, action, gun, damagedealt, iskill, position) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)'
            , 'logObjective' : 'INSERT INTO log_objective (serveridentifier, mapname, byip, byname, objectivetype) VALUES (?, ?, ?, ?, ?)'
            , 'logAccuracy' : 'INSERT INTO log_accuracy (serveridentifier, mapname, byip, byname, gun, fired, hits, position) VALUES (?, ?, ?, ?, ?, ?, ?, ?) ON DUPLICATE KEY UPDATE SET fired += ?, hits += ?'
            , 'adminLogin' : 'SELECT * FROM q3panel_forumauth WHERE serveridentifier = ? AND secretkey = ? AND used = 0 AND valid_from >= (CURRENT_TIMESTAMP - INTERVAL 10 MINUTE)'
            , 'updateSetForumKeyUsed' : 'UPDATE q3panel_forumauth SET used = 1 WHERE serveridentifier = ? AND secretkey = ?'
            , 'serverStats': 'UPDATE log_serverstats SET maxclients = ?, connectedClients = ?, mapname = ?, currtime = ?, timelimit = ?, gametype = ?, bluescore = ?, redscore = ? WHERE serveridentifier = ?'
			, 'playerStats': 'UPDATE log_playerstats SET clname = ?, team = ?, score = ?, kills = ?, deaths = ? WHERE serveridentifier = ? AND client_id = ?'
			, 'initServerStats': 'INSERT INTO log_serverstats (serveridentifier) VALUES (?)'
			, 'initPlayerStats': 'INSERT INTO log_playerstats (serveridentifier, client_id) VALUES (?, ?)'
			, 'checkServerStats': 'SELECT * FROM log_serverstats WHERE serveridentifier = ?'
			};
        if (error) {
            console.error("Error from MySQL: " + error);
            return callback({'error': true, 'errormsg': error});
        }
        if (!(action in actionQueries)) {
            console.error("Action " + action + " not defined in actionQueries.");
            return callback({'error': true, 'errormsg': 'Action ' + action + ' not defined in actionQueries'});
        }
        
        if (action == 'logAccuracy') {
            parsedData.push(parsedData[6]);
            parsedData.push(parsedData[7]);
        }
        
        parsedData.shift();
		var serverIdent = parsedData[0];
		if (action == 'serverStats') {
			conn.query(actionQueries['checkServerStats'], [serverIdent], function(queryError, queryResults) {
				if (queryResults.length === 0) {
					conn.query(actionQueries['initServerStats'], [serverIdent], function(queryError, queryResults) {console.log("queryError:" + queryError + "\r\nqueryRes: " + queryResults);});
					for (let iter = 0; iter < 64; iter++) {
						conn.query(actionQueries['initPlayerStats'], [serverIdent, iter]);
					}
				}
				
			});
			
			
			
			
			parsedData.push(serverIdent);
			parsedData.shift();
		} 
		
		if (action == 'playerStats') {
			
			parsedData.push(parsedData[0]);
			parsedData.push(parsedData[1]);
			parsedData.shift();
			parsedData.shift();
		} 
		conn.query(actionQueries[action], parsedData, function(queryError, queryResults) {
			if (queryError) {
				return {'error': true, 'errormsg': 'Error in querying mySQL'};
			}

			if (action === 'adminLogin' && queryResults.length === 1) {
				conn.query(actionQueries['updateSetForumKeyUsed'], [parsedData[0], queryResults[0].secretkey]);
			}
			
			return callback({'error': false, 'msg':queryResults});
		});
	
		

        
        conn.release();
    });
}

server.listen(process.env.SOCKET_PORT, process.env.SOCKET_HOST);
