# OS_Sincronizacion-_en_C

Para el ejercicio 2: Readers-Writers Problem
Se refiere a la sincronización de acceso a un recurso compartido (como un archivo o base de datos) donde múltiples lectores pueden leer simultáneamente, pero los escritores requieren acceso exclusivo. Existen dos variantes principales: prioridad a los lectores (los lectores pueden acceder incluso si hay escritores esperando) y prioridad a los escritores (los escritores obtienen acceso primero, bloqueando nuevos lectores hasta completar).
Para compilar este seguro de estar en la carpeta del ejericio 2 y ejecuta: 
```bash
gcc -o lectorPriority lectorPriority.c && ./lectorPriority
gcc -o escritorPriority escritorPriority.c && ./escritorPriority
```
