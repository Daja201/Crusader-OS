org 0x7C00
bits 16

%define ENDL 0x0D,0x0A

jmp start
nop

;-----------------------------------------
; BIOS Parameter Block (FAT16)
;-----------------------------------------
bdb_oem:                    db 'MSWIN4.1'           ; 8 bytes
bdb_bytes_per_sector:       dw 512
bdb_sectors_per_cluster:    db 1
bdb_reserved_sectors:       dw 1
bdb_fat_count:              db 2
bdb_dir_entries_count:      dw 0E0h
bdb_total_sectors:          dw 0
bdb_media_descriptor_type:  db 0F8h
bdb_sectors_per_fat:        dw 250
bdb_sectors_per_track:      dw 63
bdb_heads:                  dw 16
bdb_hidden_sectors:         dd 0
bdb_large_sector_count:     dd 32768               ; 16 MB

; Extended Boot Record
ebr_drive_number:           db 0
                            db 0                    ; reserved
ebr_signature:              db 29h
ebr_volume_id:              db 12h,34h,56h,78h
ebr_volume_label:           db 'NANOBYTE OS'       ; 11 chars
ebr_system_id:              db 'FAT16   '          ; 8 chars

;-----------------------------------------
; Data / Proměnné
;-----------------------------------------
file_stage2_bin:            db 'STAGE2  BIN'       ; 11 chars
stage2_cluster:             dw 0
STAGE2_LOAD_OFFSET_VAR:     dw 0                   ; dynamický offset

BUFFER_SEG    equ 0x0000
BUFFER_OFF    equ 0x0800

STAGE2_LOAD_SEGMENT equ 0x1000

msg_loading:                db 'loading', ENDL, 0
msg_read_failed:            db 'read error', ENDL,0
msg_stage2_not_found:       db 'eror',ENDL,0

;-----------------------------------------
; Start bootloader
;-----------------------------------------
start:
    xor ax, ax
    mov ds, ax
    mov es, ax

    mov ss, ax
    mov sp, 0x7C00

    push es
    push word .after
    retf

.after:
    mov [ebr_drive_number], dl
    mov si, msg_loading
    call puts

    push es
    mov ah, 08h
    int 13h
    jc floppy_error
    pop es

    and cl, 0x3F
    xor ch, ch
    mov [bdb_sectors_per_track], cx

    inc dh
    mov [bdb_heads], dh

    mov ax,[bdb_sectors_per_fat]
    mov bl,[bdb_fat_count]
    xor bh,bh
    mul bx
    add ax,[bdb_reserved_sectors]
    push ax

    mov ax,[bdb_dir_entries_count]
    shl ax,5
    xor dx,dx
    div word [bdb_bytes_per_sector]
    test dx, dx
    jz .root_dir_after
    inc ax
.root_dir_after:
    mov cl, al
    pop ax
    mov dl,[ebr_drive_number]
    mov bx, BUFFER_OFF
    mov ax, BUFFER_SEG
    mov es, ax
    call disk_read

    xor bx, bx
    mov di, BUFFER_OFF
    mov ax, BUFFER_SEG
    mov es, ax

.search_kernel:
    mov si, file_stage2_bin
    mov cx, 11
    push di
    repe cmpsb
    pop di
    je .found_kernel
    add di, 32
    inc bx
    cmp bx,[bdb_dir_entries_count]
    jl .search_kernel
    jmp kernel_not_found_error

.found_kernel:
    mov ax,[di+26]
    mov [stage2_cluster], ax

    mov ax,[bdb_reserved_sectors]
    mov bx, BUFFER_OFF
    mov ax, BUFFER_SEG
    mov es, ax
    mov cl,[bdb_sectors_per_fat]
    mov dl,[ebr_drive_number]
    call disk_read

    mov ax, STAGE2_LOAD_SEGMENT
    mov es, ax

.load_kernel_loop:
    mov ax,[stage2_cluster]
    sub ax, 2
    mov cl,[bdb_sectors_per_cluster]
    mul cl
    add ax, 533
    mov cx, 1
    mov dl,[ebr_drive_number]
    mov bx,[STAGE2_LOAD_OFFSET_VAR]
    mov ax, STAGE2_LOAD_SEGMENT
    mov es, ax
    call disk_read

    add word [STAGE2_LOAD_OFFSET_VAR], 512

    mov ax,[stage2_cluster]
    dec ax
    shl ax, 1
    mov si, BUFFER_OFF
    add si, ax
    mov ax,[es:si]

    cmp ax,0FFF8h
    jae .read_finish
    mov [stage2_cluster], ax
    jmp .load_kernel_loop

.read_finish:
    mov dl, [ebr_drive_number]
    mov ax, STAGE2_LOAD_SEGMENT
    mov ds, ax
    mov es, ax

    mov ax, STAGE2_LOAD_SEGMENT    ; segment stage2
    mov ds, ax
    mov es, ax

    mov bx, [STAGE2_LOAD_OFFSET_VAR]  ; offset stage2

    push ax        ; segment
    push bx        ; offset
    retf           ; far return → skok na segment:offset
             


;-----------------------------------------
; Error handlers
;-----------------------------------------
floppy_error:
    mov si, msg_read_failed
    call puts
    jmp wait_key_and_reboot

kernel_not_found_error:
    mov si, msg_stage2_not_found
    call puts
    jmp wait_key_and_reboot

wait_key_and_reboot:
    mov ah,0
    int 16h
    jmp 0FFFFh:0

;-----------------------------------------
; Basic printing routine
;-----------------------------------------
puts:
    push si
.loop:
    lodsb
    or al, al
    jz .done
    mov ah, 0Eh
    mov bh, 0
    int 10h
    jmp .loop
.done:
    pop si
    ret

;-----------------------------------------
; Disk routines
;-----------------------------------------
lba_to_chs:
    push bx
    push cx
    push dx
    mov bx,[bdb_sectors_per_track]
    xor dx, dx
    div bx
    mov cl, dl
    inc cl
    mov bx,[bdb_heads]
    xor dx, dx
    div bx
    mov dh, dl
    mov ch, al
    mov bx, ax
    shr bx,8
    and bl,3
    shl bl,6
    or cl, bl
    pop dx
    pop cx
    pop bx
    ret

disk_read:
    push ax
    push bx
    push cx
    push dx
    push si
    push di

    mov bh, cl
    push ax
    call lba_to_chs
    mov ah,02h
    mov al,bh
    mov dl,[ebr_drive_number]

    mov si, 3
.retry_disk:
    clc
    int 13h
    jc .retry_failed
    pop di
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    ret

.retry_failed:
    dec si
    jnz .retry_disk
    pop di
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    jmp floppy_error

disk_reset:
    pusha
    mov ah,0
    stc
    int 13h
    jc floppy_error
    popa
    ret

;-----------------------------------------
; Boot sector padding
;-----------------------------------------
times 510-($-$$) db 0
dw 0AA55h
