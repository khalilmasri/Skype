--  LIST A USER CONTACTS
SELECT DISTINCT U.username,

  (
    SELECT STRING_AGG(U1.id::VARCHAR, ',') AS id -- casting INTEGER to aggregate
    FROM contacts C1
    INNER JOIN users U1
    ON C1.contact_id = U1.id
    WHERE U.id = C1.user_id
  ),
  ( 
    SELECT DISTINCT STRING_AGG(U2.username, ',') AS contacts
    FROM contacts C2
    INNER JOIN users U2
    ON C2.contact_id = U2.id
    WHERE U.id = C2.user_id
  ),
  (
    SELECT DISTINCT STRING_AGG(U4.online::VARCHAR, ',') AS online
    FROM contacts C4
    INNER JOIN users U4
    ON C4.contact_id = U4.id
    WHERE U.id = C4.user_id
  )
  
FROM contacts C
JOIN users U
ON C.user_id = U.id
WHERE U.username = 'khalil'; -- substitute username to search for




