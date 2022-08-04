--  FIND A USER CONTACTS
SELECT U.username, U.password, U.online, U.address
FROM contacts C
INNER JOIN users U
ON C.contact_id = U.id
WHERE C.user_id = 1 AND U.username = 'mario' -- substitue user_id and U.username with params

