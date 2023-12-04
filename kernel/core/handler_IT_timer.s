.text
    .globl handler_IT_timer
handler_IT_timer:
# sauvegarde des registres
    pusha
    call handler_timer
# restauration des registres
    popa
    iret