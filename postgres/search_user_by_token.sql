
SELECT T.user_id, U.username, U.online, U.address
FROM tokens AS T
JOIN users AS U ON T.user_id = U.id
WHERE T.token = 'abcabc'; -- substitute with a token
