##########             Diego Roberto Roche Palacios                     ###########
##########             Percy Matthew Jacobs Orellana                    ###########
##########             Josue Daniel Barrios Morales                     ###########

################################### Librerias ######################################
import tkinter as tk
from tkinter import *
from tkinter import messagebox
from tkinter import ttk
from tkinter import filedialog
import serial
import PIL
from PIL import ImageTk as itk
from PIL import Image
from PIL import *
import time
import sqlite3

################################### Variables ####################################### 
serial_data = ""
db_name = 'database.db'

####################################################################################
###########                                                              ###########
###########              METODOS PARA LA CONEXIÓN SERIAL                 ###########
###########                                                              ###########  
####################################################################################
def cnct():
    global prto
    global ser
    if (prto.get() !='') :
        ser = serial.Serial(prto.get(),9600)
        ser.setDTR(False)
        time.sleep(1)
        ser.flushInput()
        ser.setDTR(True) 
    else:
        messagebox.showwarning("Advertencia","Ingresar campos válidos")
        return 

def lectura():
    global serial_data
    global ser
    while(1):
        serial_data=ser.read()
        print(serial_data)
    window.after(10, lectura)


####################################################################################
###########                                                              ###########
###########              METODOS PARA LA BASE DE DATOS                   ###########
###########                                                              ###########  
####################################################################################
def run_query(query,parameters=()):
    
    with sqlite3.connect(db_name) as conn:
        cursor = conn.cursor()
        result = cursor.execute(query,parameters)
        conn.commit()
    return result

def get_muestra():
    global tabla
    records = tabla.get_children()  #Limpieza de tabla
    for element in records:
        tabla.delete(element)
    query = 'SELECT * FROM Muestras ORDER BY id DESC' #consulta de la tabla
    db_rows = run_query(query)
    for row in db_rows:
        tabla.insert("","end", text="", values=(row[1],row[2],row[3]))

def validation():
    global nombre_muestra
    return len(nombre_muestra.get()) != 0 

def add_new():
    global nombre_muestra
    global ph
    global temperatura
    if validation():
        query='INSERT INTO Muestras VALUES(NULL,?,?,?)'
        parametros=(nombre_muestra.get(),ph.get(),temperatura.get())
        run_query(query,parametros)
    else:
        messagebox.showwarning("Advertencia","Ingresar un nombre válido")
    get_muestra()

def delete_this():
    global nombree
    nombree=tabla.item(tabla.selection())["values"]
    longitud=len(nombree)
    if longitud >=1:
        eliminame=nombree[0]
        query="DELETE FROM Muestras WHERE Nombre=?"
        run_query(query,(eliminame,))
    else:
        messagebox.showwarning("Advertencia","Seleccionar item a eliminar")
        return
    get_muestra()

def edit_this():
    global nombre_edicion
    global nuevo_nombre_muestra
    global ventana_edicion
    query = "UPDATE Muestras SET Nombre=? WHERE Nombre =?"
    #parameters =
    run_query(query,(nuevo_nombre_muestra.get(),nombre_edicion))
    ventana_edicion.destroy()
    get_muestra()
    
def edicion():
    global nombre_edicion
    global nuevo_nombre_muestra
    global ventana_edicion
    #nombre_edicion=nombre_muestra
    nuevo_nombre_muestra=tk.StringVar()
    try:
        nombre_edicion=tabla.item(tabla.selection())["values"][0]
    except IndexError as e:
        messagebox.showwarning("Advertencia","Seleccionar item a editar")
        return
    
    nombre_edicion=tabla.item(tabla.selection())["values"][0]
    ventana_edicion = Toplevel()
    ventana_edicion.title = "Edición del nombre"
    ventana_edicion.geometry("400x175+450+250")
    ventana_edicion.configure(background="pale goldenrod")
    Label(ventana_edicion,text="Edición de nombre", background="pale goldenrod",font=("comicsans",18)).place(x=25,y=20)
    Label(ventana_edicion,text="Nombre anterior", background="pale goldenrod",font=("comicsans",12)).place(x=15,y=80)
    tk.Entry(ventana_edicion,textvariable=StringVar(ventana_edicion, value= nombre_edicion),font=("comicsans",12),state="readonly").place(x=135,y=80)
    Label(ventana_edicion,text="Nombre nuevo", background="pale goldenrod",font=("comicsans",12)).place(x=15,y=110)
    tk.Entry(ventana_edicion,textvariable=nuevo_nombre_muestra,font=("comicsans",12)).place(x=135,y=110)
    Button(ventana_edicion,command=edit_this,text="ACTUALIZAR",background="snow4",font=("comicsans",12)).place(x=60,y=135)
    

        

####################################################################################
###########                                                              ###########
###########           METODOS PARA LA INTERFAZ GRAFICA                   ###########
###########                                                              ###########  
####################################################################################

def ventana1 (window):
    global tabla
    global nombre_muestra
    global ph
    global temperatura
    global prto
    #Imagen de la USAC
    img = Image.open("escudo10.png")
    img = img.resize((50,50),Image.ANTIALIAS)
    imagenLogo = ImageTk.PhotoImage(img)
    imgLogoLabel= tk.Label(window,image=imagenLogo)
    imgLogoLabel.image = imagenLogo
    imgLogoLabel.place(x=5,y=10)

    #Titulos y demás textos
    ph=tk.StringVar()
    ph.set("7")
    temperatura=tk.StringVar()
    temperatura.set("30")
    Label(window,text="Universidad de San Carlos de Guatemala", background="pale goldenrod", font=("comicsans",15)).place(x=65,y=10)
    Label(window, text="FIUSAC - CCQQFAR ", background="pale goldenrod", font =("comicsans",15)).place(x=65,y=35)
    Label(window,text="Puerto serial", background="pale goldenrod",font=("comicsans",12)).place(x=30,y=80)
    Label(window,text="Nombre de la", background="pale goldenrod",font=("comicsans",12)).place(x=30,y=110)
    Label(window,text="muestra", background="pale goldenrod",font=("comicsans",12)).place(x=30,y=130)
    Label(window,text="Lectura de Ph", background="pale goldenrod",font=("comicsans",12)).place(x=50,y=160)
    Label(window,text=str(ph), background="pale goldenrod",font=("comicsans",12)).place(x=90,y=185)
    Label(window, text="Temperatura", background="pale goldenrod", font=("comicsans", 12)).place(x=200, y=160)
    Label(window, text= str(temperatura) + "°C", background="pale goldenrod", font=("comicsans", 12)).place(x=225, y=185)
    
    #Botones y entradas de texto
    prto=tk.StringVar()
    prto.set("COM")
    nombre_muestra=tk.StringVar()
    tk.Entry(window,textvariable=prto,font=("comicsans",12)).place(x=130,y=80)
    tk.Entry(window,textvariable=nombre_muestra,font=("comicsans",12)).place(x=130,y=120)
    #Button(window,command=cnct, text=" CONECTAR ",background="snow4",font=("comicsans",12)).place(x=347,y=70)
    Button(window,command=add_new,text=" REGISTRAR ",background="snow4",font=("comicsans",12)).place(x=347,y=105)    
    Button(window,command=edicion, text="      EDITAR     ",background="snow4",font=("comicsans",12)).place(x=345,y=140)
    Button(window,command=delete_this ,text="  REMOVER   ",background="snow4",font=("comicsans",12)).place(x=345,y=175)

    #Tabla
    tabla = ttk.Treeview(window)
    tabla['columns']=("Nombre","LecturaPh","Temperatura")
    tabla.place(x=30,y=220)
    tabla.heading("#0",text="",anchor="center")
    tabla.column("#0",anchor="center",width=5,stretch=tk.NO)
    tabla.heading("Nombre", text="Nombre", anchor="center")
    tabla.column("Nombre",anchor="center",width=160)
    tabla.heading("LecturaPh", text="Lectura de PH", anchor="center")
    tabla.column("LecturaPh",anchor="center",width=120)
    tabla.heading("Temperatura", text="Temperatura", anchor="center")
    tabla.column("Temperatura",anchor="center",width=120)
    
    get_muestra()
    


if __name__ == "__main__":
    window=tk.Tk()
    window.title("Potenciometro USAC")
    window.geometry("465x480+400+200")
    window.configure(background="pale goldenrod")
    application=ventana1(window)
    #window.after(100, lectura)
    window.mainloop()
    