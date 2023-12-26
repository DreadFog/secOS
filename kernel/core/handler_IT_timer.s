.text
    .globl handler_IT_timer
handler_IT_timer:
# Registers saving
    pusha
    call handler_timer
# Registers retrieval
    popa
    iret