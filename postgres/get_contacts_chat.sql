SELECT DISTINCT id, created_at, delivered, text
FROM chats
WHERE sender_id = 1 AND recipient_id = 2;

