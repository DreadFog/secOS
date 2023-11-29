void irq0_handler()
{
	asm volatile("pusha \t\n");
	/* TODO: 
		- switch tâche 1 <-> tâche 2 
		- savoir si on a interrompu le noyau ou une tâche utilisateur
	*/
	// Rendre la main à tp()
	asm volatile("iret \t\n");
}
