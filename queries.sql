--  LIST A USER CONTACTS

SELECT U.username,

    (SELECT SRZ.id
     FROM contacts CONT
     INNER JOIN users SRZ
     ON CONT.contact_id = SRZ.id
     WHERE CONT.contact_id = C.contact_id
    ),

    (
      SELECT DISTINCT STRING_AGG(USR.username, ',') AS contacts
     FROM contacts CONT
     INNER JOIN users USR
     ON CONT.contact_id = USR.id
     WHERE CONT.contact_id = C.contact_id
    ),

   (
      SELECT DISTINCT STRING_AGG(USR.address, ',') AS address
     FROM contacts CONT
     INNER JOIN users USR
     ON CONT.contact_id = USR.id
     WHERE CONT.contact_id = C.contact_id
    ),

    (
     SELECT SR.online
     FROM contacts CONT
     INNER JOIN users SR
     ON CONT.contact_id = SR.id
     WHERE CONT.contact_id = C.contact_id
    )

FROM contacts C
JOIN users U
ON C.user_id = U.id
WHERE U.username = 'khalil';



--  FIND A USER CONTACTS
