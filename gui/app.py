"""
Interfaz gráfica del mini sistema de análisis genómico.

Esta ventana NO hace cálculos. Ejecuta el programa en C++ con
"./genomica --gui", le envía comandos (uno por línea) y muestra las
respuestas JSON que devuelve. Ver src/modoGUI/ModoGUI.h para la lista de comandos.

Solo usa la biblioteca estándar de Python (tkinter viene incluido).

    make gui            # compila el C++ y abre la ventana
    python3 gui/app.py  # si ya está compilado
"""

import json
import math
import subprocess
import sys
import tkinter as tk
import tkinter.font as tkfont
from pathlib import Path
from tkinter import filedialog, messagebox, ttk

RAIZ = Path(__file__).resolve().parent.parent
EJECUTABLE = RAIZ / "genomica"
ARCHIVO_POR_DEFECTO = RAIZ / "data" / "HIV2_ALL_2021_env_DNA.2000.fasta"

# Paleta de colores
FONDO = "#f4f6f8"
PANEL = "#ffffff"
BARRA = "#16232f"
BARRA_TEXTO = "#c9d4de"
BARRA_ACTIVA = "#24384a"
ACENTO = "#0e7c6b"
ACENTO_CLARO = "#d7efe9"
TEXTO = "#1d2730"
TEXTO_SUAVE = "#5f6d7a"
BORDE = "#dde3e8"
ERROR = "#b42318"

# Fuentes: se llenan en elegir_fuentes() cuando ya existe la ventana.
F = {}

# Un color por nucleótido para mostrar las secuencias
COLOR_BASE = {"A": "#1f8a4c", "C": "#2563b0", "G": "#b7791f", "T": "#c0392b"}


def elegir_fuentes(raiz):
    """Usa la primera fuente instalada de cada lista (cambia según el sistema)."""
    instaladas = {f.lower(): f for f in tkfont.families(raiz)}

    def primera(opciones, respaldo):
        for nombre in opciones:
            if nombre.lower() in instaladas:
                return instaladas[nombre.lower()]
        return tkfont.nametofont(respaldo, root=raiz).actual("family")

    sans = primera(["Inter", "Segoe UI", "SF Pro Text", "Helvetica Neue", "Ubuntu", "Cantarell",
                    "Noto Sans", "DejaVu Sans", "Helvetica", "Arial"], "TkDefaultFont")
    mono = primera(["JetBrains Mono", "Cascadia Mono", "Consolas", "Menlo", "Ubuntu Mono",
                    "DejaVu Sans Mono", "Noto Sans Mono", "Courier New"], "TkFixedFont")
    for nombre, familia, tamano, peso in [
        ("normal", sans, 10, "normal"), ("chica", sans, 9, "normal"), ("mini", sans, 8, "normal"),
        ("navegacion", sans, 11, "normal"), ("cabecera", sans, 9, "bold"),
        ("resultado", sans, 11, "bold"), ("valor", sans, 17, "bold"), ("marca", sans, 16, "bold"),
        ("titulo", sans, 18, "bold"), ("mono", mono, 11, "normal"), ("mono_negrita", mono, 11, "bold"),
        ("mono_chica", mono, 10, "normal"), ("mono_chica_negrita", mono, 10, "bold"),
    ]:
        F[nombre] = tkfont.Font(raiz, family=familia, size=tamano, weight=peso)
    # Las fuentes por defecto de Tk también pasan a usar la familia elegida.
    tkfont.nametofont("TkDefaultFont", root=raiz).configure(family=sans, size=10)
    tkfont.nametofont("TkTextFont", root=raiz).configure(family=sans, size=10)
    tkfont.nametofont("TkHeadingFont", root=raiz).configure(family=sans)


# =============================================================================
# Comunicación con el programa en C++
# =============================================================================
class MotorCpp:
    """Mantiene abierto el proceso ./genomica --gui y le envía comandos."""

    def __init__(self):
        if not EJECUTABLE.exists():
            # Intentamos compilar automáticamente.
            subprocess.run(["make"], cwd=RAIZ, check=False)
        if not EJECUTABLE.exists():
            raise RuntimeError("No se encontró ./genomica. Ejecuta 'make' primero.")

        self.proceso = subprocess.Popen(
            [str(EJECUTABLE), "--gui"],
            cwd=RAIZ,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
            text=True,
            encoding="utf-8",
            bufsize=1,  # Envía cada línea apenas se escribe
        )

    def enviar(self, *partes):
        """Envía un comando y devuelve la respuesta como diccionario."""
        limpias = [str(p).replace("\t", " ").replace("\n", " ") for p in partes]
        try:
            self.proceso.stdin.write("\t".join(limpias) + "\n")
            self.proceso.stdin.flush()
            linea = self.proceso.stdout.readline()
        except (BrokenPipeError, OSError):
            linea = ""
        if not linea:
            return {"ok": False, "error": "El programa en C++ se cerró inesperadamente."}
        return json.loads(linea)

    def cerrar(self):
        try:
            self.enviar("salir")
            self.proceso.wait(timeout=2)
        except Exception:
            self.proceso.kill()


# =============================================================================
# Widgets de apoyo
# =============================================================================
def crear_tabla(padre, columnas, alturas=12):
    """Treeview con barra de desplazamiento. columnas = [(id, título, ancho, ancla)]."""
    marco = ttk.Frame(padre, style="Panel.TFrame")
    tabla = ttk.Treeview(marco, columns=[c[0] for c in columnas], show="headings", height=alturas)
    for ident, titulo, ancho, ancla in columnas:
        tabla.heading(ident, text=titulo, command=lambda c=ident: ordenar_tabla(tabla, c))
        tabla.column(ident, width=ancho, anchor=ancla, stretch=(ancla == "w"))
    barra = ttk.Scrollbar(marco, orient="vertical", command=tabla.yview)
    tabla.configure(yscrollcommand=barra.set)
    tabla.grid(row=0, column=0, sticky="nsew")
    barra.grid(row=0, column=1, sticky="ns")
    marco.rowconfigure(0, weight=1)
    marco.columnconfigure(0, weight=1)
    tabla.tag_configure("vacia", foreground="#a7b1ba")
    return marco, tabla


def ordenar_tabla(tabla, columna):
    """Ordena las filas al hacer clic en el título (clic otra vez = invierte)."""
    filas = [(tabla.set(i, columna), i) for i in tabla.get_children("")]

    def clave(valor):
        try:
            return (0, float(valor[0]))
        except ValueError:
            return (1, valor[0])

    invertir = getattr(tabla, "_orden", None) == columna
    filas.sort(key=clave, reverse=invertir)
    for pos, (_, item) in enumerate(filas):
        tabla.move(item, "", pos)
    tabla._orden = None if invertir else columna


class Tarjetas(ttk.Frame):
    """Fila de indicadores: título pequeño arriba y valor grande abajo."""

    def __init__(self, padre, titulos):
        super().__init__(padre, style="Fondo.TFrame")
        self.valores = {}
        for i, titulo in enumerate(titulos):
            caja = ttk.Frame(self, style="Tarjeta.TFrame", padding=(14, 10))
            caja.grid(row=0, column=i, sticky="nsew", padx=(0 if i == 0 else 10, 0))
            self.columnconfigure(i, weight=1)
            ttk.Label(caja, text=titulo, style="TarjetaTitulo.TLabel").pack(anchor="w")
            valor = ttk.Label(caja, text="-", style="TarjetaValor.TLabel")
            valor.pack(anchor="w")
            self.valores[titulo] = valor

    def poner(self, **datos):
        for titulo, valor in datos.items():
            self.valores[titulo.replace("_", " ")].configure(text=valor)


def texto_secuencia(padre, alto=8):
    """Text de solo lectura con colores por nucleótido."""
    marco = ttk.Frame(padre, style="Panel.TFrame")
    caja = tk.Text(marco, height=alto, wrap="char", font=F["mono"], relief="flat",
                   bg=PANEL, fg=TEXTO, padx=10, pady=8, highlightthickness=1,
                   highlightbackground=BORDE, highlightcolor=BORDE)
    barra = ttk.Scrollbar(marco, orient="vertical", command=caja.yview)
    caja.configure(yscrollcommand=barra.set, state="disabled")
    caja.grid(row=0, column=0, sticky="nsew")
    barra.grid(row=0, column=1, sticky="ns")
    marco.rowconfigure(0, weight=1)
    marco.columnconfigure(0, weight=1)
    for base, color in COLOR_BASE.items():
        caja.tag_configure(base, foreground=color)
    caja.tag_configure("otra", foreground="#ffffff", background="#8a96a3")
    return marco, caja


def mostrar_secuencia(caja, secuencia):
    caja.configure(state="normal")
    caja.delete("1.0", "end")
    # Se agrupan las letras iguales seguidas para insertar menos veces.
    inicio = 0
    for i in range(1, len(secuencia) + 1):
        if i == len(secuencia) or secuencia[i] != secuencia[inicio]:
            base = secuencia[inicio]
            caja.insert("end", secuencia[inicio:i], base if base in COLOR_BASE else "otra")
            inicio = i
    caja.configure(state="disabled")


# =============================================================================
# Páginas
# =============================================================================
class Pagina(ttk.Frame):
    titulo = ""
    subtitulo = ""

    def __init__(self, app):
        super().__init__(app.contenido, style="Fondo.TFrame", padding=(28, 22))
        self.app = app
        ttk.Label(self, text=self.titulo, style="Titulo.TLabel").pack(anchor="w")
        ttk.Label(self, text=self.subtitulo, style="Subtitulo.TLabel").pack(anchor="w", pady=(2, 16))

    def al_mostrar(self):
        """Se llama cada vez que se abre la página."""

    def pedir(self, *comando):
        """Envía un comando al C++; si hay error lo muestra y devuelve None."""
        self.app.configure(cursor="watch")
        self.app.update_idletasks()
        try:
            r = self.app.motor.enviar(*comando)
        finally:
            self.app.configure(cursor="")
        if not r.get("ok"):
            self.app.estado(r.get("error", "Error desconocido"), error=True)
            return None
        return r

    def fila_controles(self):
        fila = ttk.Frame(self, style="Fondo.TFrame")
        fila.pack(fill="x", pady=(0, 14))
        return fila


class PaginaArchivo(Pagina):
    titulo = "Archivo FASTA"
    subtitulo = "Parte a (parser) y parte b (tabla hash 1: ID -> secuencia, linear probing)"

    def __init__(self, app):
        super().__init__(app)
        fila = self.fila_controles()
        self.ruta = tk.StringVar(value=str(ARCHIVO_POR_DEFECTO.relative_to(RAIZ)))
        ttk.Entry(fila, textvariable=self.ruta).pack(side="left", fill="x", expand=True)
        ttk.Button(fila, text="Examinar...", command=self.examinar).pack(side="left", padx=8)
        ttk.Button(fila, text="Cargar", style="Acento.TButton", command=self.cargar).pack(side="left")

        self.tarjetas = Tarjetas(self, ["Secuencias", "Casillas", "Factor de carga",
                                        "Colisiones", "Redimensionamientos"])
        self.tarjetas.pack(fill="x", pady=(0, 14))

        marco, self.tabla = crear_tabla(self, [
            ("bin", "Bin", 60, "center"),
            ("id", "Key (ID)", 220, "w"),
            ("largo", "Bases", 80, "e"),
            ("inicio", "Valor (inicio de la secuencia)", 420, "w"),
        ])
        marco.pack(fill="both", expand=True)
        self.tabla.bind("<Double-1>", self.abrir_secuencia)
        ttk.Label(self, text="Doble clic en una fila para ver la secuencia completa.",
                  style="Nota.TLabel").pack(anchor="w", pady=(6, 0))

    def examinar(self):
        ruta = filedialog.askopenfilename(
            initialdir=RAIZ / "data", title="Elegir archivo FASTA",
            filetypes=[("FASTA", "*.fasta *.fa *.fas *.fna *.txt"), ("Todos", "*")])
        if ruta:
            self.ruta.set(ruta)
            self.cargar()

    def cargar(self):
        r = self.pedir("cargar", self.ruta.get().strip())
        if r:
            self.app.archivo_cargado(r)
            self.refrescar()

    def refrescar(self):
        r = self.pedir("tabla1")
        if not r:
            return
        self.tarjetas.poner(Secuencias=r["cantidad"], Casillas=r["tamano"],
                            Factor_de_carga=f'{r["factorCarga"]:.3f}', Colisiones=r["colisiones"],
                            Redimensionamientos=r["redimensionamientos"])
        self.tabla.delete(*self.tabla.get_children())
        for i, c in enumerate(r["casillas"]):
            if c is None:
                self.tabla.insert("", "end", values=(i, "-", "", ""), tags=("vacia",))
            else:
                extra = "..." if c["largo"] > len(c["inicio"]) else ""
                self.tabla.insert("", "end", values=(i, c["id"], c["largo"], c["inicio"] + extra))

    def al_mostrar(self):
        if self.app.ids and not self.tabla.get_children():
            self.refrescar()

    def abrir_secuencia(self, _evento):
        item = self.tabla.focus()
        if item and "vacia" not in self.tabla.item(item, "tags"):
            self.app.ir_a("buscar", id=self.tabla.set(item, "id"))


class PaginaBuscar(Pagina):
    titulo = "Buscar secuencia"
    subtitulo = "Parte b: búsqueda por ID en la tabla hash 1"

    def __init__(self, app):
        super().__init__(app)
        fila = self.fila_controles()
        ttk.Label(fila, text="ID", style="Fondo.TLabel").pack(side="left", padx=(0, 8))
        self.id = ttk.Combobox(fila, width=40)
        self.id.pack(side="left", fill="x", expand=True)
        self.id.bind("<Return>", lambda e: self.buscar())
        self.id.bind("<<ComboboxSelected>>", lambda e: self.buscar())
        ttk.Button(fila, text="Buscar", style="Acento.TButton", command=self.buscar).pack(side="left", padx=(8, 0))

        self.info = ttk.Label(self, text="", style="Fondo.TLabel")
        self.info.pack(anchor="w", pady=(0, 8))
        marco, self.caja = texto_secuencia(self, alto=16)
        marco.pack(fill="both", expand=True)

        leyenda = ttk.Frame(self, style="Fondo.TFrame")
        leyenda.pack(anchor="w", pady=(8, 0))
        for base, color in COLOR_BASE.items():
            tk.Label(leyenda, text=f" {base} ", bg=color, fg="white", font=F["mono_chica_negrita"]).pack(side="left", padx=(0, 4))
        ttk.Label(leyenda, text="  gris = letra ambigua (N, R, Y, ...)", style="Nota.TLabel").pack(side="left")

    def al_mostrar(self, id=None):
        self.id["values"] = self.app.ids
        if id:
            self.id.set(id)
            self.buscar()

    def buscar(self):
        r = self.pedir("buscar", self.id.get().strip())
        if not r:
            return
        sec = r["secuencia"]
        conteo = {b: sec.count(b) for b in "ACGT"}
        otras = len(sec) - sum(conteo.values())
        gc = (conteo["G"] + conteo["C"]) / len(sec) * 100 if sec else 0
        detalle = "   ".join(f"{b}: {n}" for b, n in conteo.items())
        self.info.configure(text=f">{r['id']}   ·   {r['largo']} bases   ·   {detalle}   ·   "
                                 f"ambiguas: {otras}   ·   GC: {gc:.1f}%")
        mostrar_secuencia(self.caja, sec)
        self.app.estado(f"Secuencia {r['id']} encontrada.")


class PaginaKmers(Pagina):
    titulo = "Tabla de k-mers"
    subtitulo = "Parte b: tabla hash 2 (k-mer -> conteo) con double hashing, tamaño 4^k"

    def __init__(self, app):
        super().__init__(app)
        fila = self.fila_controles()
        ttk.Label(fila, text="Secuencia", style="Fondo.TLabel").pack(side="left", padx=(0, 8))
        self.id = ttk.Combobox(fila, width=34)
        self.id.pack(side="left")
        ttk.Label(fila, text="k", style="Fondo.TLabel").pack(side="left", padx=(16, 8))
        self.k = tk.IntVar(value=3)
        ttk.Spinbox(fila, from_=1, to=10, width=4, textvariable=self.k).pack(side="left")
        ttk.Button(fila, text="Construir tabla", style="Acento.TButton",
                   command=self.construir).pack(side="left", padx=(12, 0))

        self.tarjetas = Tarjetas(self, ["Casillas (4^k)", "k-mers totales", "k-mers distintos",
                                        "Factor de carga", "Colisiones"])
        self.tarjetas.pack(fill="x", pady=(0, 14))

        busqueda = ttk.Frame(self, style="Fondo.TFrame")
        busqueda.pack(fill="x", pady=(0, 10))
        ttk.Label(busqueda, text="Buscar k-mer", style="Fondo.TLabel").pack(side="left", padx=(0, 8))
        self.kmer = ttk.Entry(busqueda, width=16, font=F["mono"])
        self.kmer.pack(side="left")
        self.kmer.bind("<Return>", lambda e: self.buscar_kmer())
        ttk.Button(busqueda, text="Buscar", command=self.buscar_kmer).pack(side="left", padx=8)
        self.resultado = ttk.Label(busqueda, text="", style="Resultado.TLabel")
        self.resultado.pack(side="left", padx=8)

        marco, self.tabla = crear_tabla(self, [
            ("bin", "Bin", 90, "center"),
            ("kmer", "Key (k-mer)", 200, "w"),
            ("conteo", "Valor (conteo)", 120, "e"),
        ])
        marco.pack(fill="both", expand=True)
        ttk.Label(self, text="Solo se muestran las casillas ocupadas. Clic en un título para ordenar.",
                  style="Nota.TLabel").pack(anchor="w", pady=(6, 0))

    def al_mostrar(self):
        self.id["values"] = self.app.ids
        if not self.id.get() and self.app.ids:
            self.id.set(self.app.ids[0])

    def construir(self):
        try:
            k = int(self.k.get())
        except (tk.TclError, ValueError):
            self.app.estado("k debe ser un número entero.", error=True)
            return
        r = self.pedir("kmers", self.id.get().strip(), k)
        if not r:
            return
        self.app.kmers_construidos(r)
        r = self.pedir("tabla2")
        if not r:
            return
        self.tarjetas.poner(**{"Casillas (4^k)": r["tamano"], "k-mers totales": r["totales"],
                               "k-mers distintos": r["distintos"],
                               "Factor de carga": f'{r["factorCarga"]:.3f}',
                               "Colisiones": r["colisiones"]})
        self.tabla.delete(*self.tabla.get_children())
        for c in r["casillas"]:
            self.tabla.insert("", "end", values=(c["bin"], c["kmer"], c["conteo"]))
        self.resultado.configure(text="")

    def buscar_kmer(self):
        r = self.pedir("kmer", self.kmer.get().strip())
        if r:
            self.resultado.configure(text=f'{r["kmer"]} aparece {r["conteo"]} veces')


class PaginaTop(Pagina):
    titulo = "k-mers más frecuentes"
    subtitulo = "Parte c: max-heap construido con los pares (k-mer, conteo) de la tabla hash 2"

    def __init__(self, app):
        super().__init__(app)
        fila = self.fila_controles()
        ttk.Label(fila, text="n", style="Fondo.TLabel").pack(side="left", padx=(0, 8))
        self.n = tk.IntVar(value=10)
        ttk.Spinbox(fila, from_=1, to=100000, width=7, textvariable=self.n).pack(side="left")
        ttk.Button(fila, text="Calcular top n", style="Acento.TButton",
                   command=self.calcular).pack(side="left", padx=(12, 0))
        self.info = ttk.Label(fila, text="", style="Nota.TLabel")
        self.info.pack(side="left", padx=16)

        cuerpo = ttk.Frame(self, style="Fondo.TFrame")
        cuerpo.pack(fill="both", expand=True)
        marco, self.tabla = crear_tabla(cuerpo, [
            ("pos", "#", 50, "center"),
            ("kmer", "k-mer", 150, "w"),
            ("conteo", "Conteo", 90, "e"),
        ])
        marco.pack(side="left", fill="y")
        self.lienzo = tk.Canvas(cuerpo, bg=PANEL, highlightthickness=1, highlightbackground=BORDE)
        self.lienzo.pack(side="left", fill="both", expand=True, padx=(14, 0))
        self.lienzo.bind("<Configure>", lambda e: self.dibujar())
        self.datos = []

    def al_mostrar(self):
        self.info.configure(text=self.app.descripcion_kmers or "Primero construye la tabla de k-mers.")

    def calcular(self):
        try:
            n = int(self.n.get())
        except (tk.TclError, ValueError):
            self.app.estado("n debe ser un número entero.", error=True)
            return
        r = self.pedir("top", n)
        if not r:
            return
        self.datos = r["top"]
        self.tabla.delete(*self.tabla.get_children())
        for i, d in enumerate(self.datos, 1):
            self.tabla.insert("", "end", values=(i, d["kmer"], d["conteo"]))
        self.app.estado(f'Top {len(self.datos)} de {r["distintos"]} {r["k"]}-mers distintos.')
        self.dibujar()

    def dibujar(self):
        """Gráfico de barras horizontales con los primeros 30."""
        c = self.lienzo
        c.delete("all")
        datos = self.datos[:30]
        if not datos:
            c.create_text(c.winfo_width() / 2, c.winfo_height() / 2, text="Sin datos",
                          fill=TEXTO_SUAVE, font=F["navegacion"])
            return
        ancho, alto = c.winfo_width(), c.winfo_height()
        margen_izq, margen_der, arriba = 110, 50, 16
        fila = min(26, (alto - 2 * arriba) / len(datos))
        maximo = max(d["conteo"] for d in datos)
        for i, d in enumerate(datos):
            y = arriba + i * fila
            largo = (ancho - margen_izq - margen_der) * d["conteo"] / maximo
            c.create_text(margen_izq - 10, y + fila / 2, text=d["kmer"], anchor="e",
                          font=F["mono_chica"], fill=TEXTO)
            c.create_rectangle(margen_izq, y + fila * 0.18, margen_izq + largo, y + fila * 0.82,
                               fill=ACENTO, outline="")
            c.create_text(margen_izq + largo + 6, y + fila / 2, text=d["conteo"], anchor="w",
                          font=F["chica"], fill=TEXTO_SUAVE)


class PaginaGrafo(Pagina):
    titulo = "Grafo de 2-mers"
    subtitulo = "Parte d: pares sin solapamiento, arcos i -> j con peso 1 / (j - i)^beta (Khan et al., 2024)"

    def __init__(self, app):
        super().__init__(app)
        fila = self.fila_controles()
        ttk.Label(fila, text="ID o secuencia", style="Fondo.TLabel").pack(side="left", padx=(0, 8))
        self.entrada = ttk.Combobox(fila, width=40)
        self.entrada.set("TCATTAGCAGTTAGCAGC")
        self.entrada.pack(side="left", fill="x", expand=True)
        ttk.Label(fila, text="beta", style="Fondo.TLabel").pack(side="left", padx=(16, 8))
        self.beta = ttk.Entry(fila, width=6)
        self.beta.insert(0, "1")
        self.beta.pack(side="left")
        ttk.Button(fila, text="Construir grafo", style="Acento.TButton",
                   command=self.construir).pack(side="left", padx=(12, 0))
        self.entrada.bind("<Return>", lambda e: self.construir())

        self.info = ttk.Label(self, text="", style="Nota.TLabel")
        self.info.pack(anchor="w", pady=(0, 8))

        cuerpo = ttk.Frame(self, style="Fondo.TFrame")
        cuerpo.pack(fill="both", expand=True)
        marco, self.pares = crear_tabla(cuerpo, [
            ("pos", "Posición", 70, "center"),
            ("par", "2-mer", 120, "w"),
        ])
        marco.pack(side="left", fill="y")
        self.pares.tag_configure("invalido", foreground=ERROR)

        pestanas = ttk.Notebook(cuerpo)
        pestanas.pack(side="left", fill="both", expand=True, padx=(14, 0))
        self.lienzo_grafo = tk.Canvas(pestanas, bg=PANEL, highlightthickness=0)
        self.lienzo_matriz = tk.Canvas(pestanas, bg=PANEL, highlightthickness=0)
        pestanas.add(self.lienzo_grafo, text="  Grafo  ")
        pestanas.add(self.lienzo_matriz, text="  Matriz de adyacencia  ")
        self.lienzo_grafo.bind("<Configure>", lambda e: self.dibujar_grafo())
        self.lienzo_matriz.bind("<Configure>", lambda e: self.dibujar_matriz())
        self.datos = None

    def al_mostrar(self):
        self.entrada["values"] = ["TCATTAGCAGTTAGCAGC"] + self.app.ids

    def construir(self):
        r = self.pedir("grafo", self.beta.get().strip(), self.entrada.get().strip())
        if not r:
            return
        self.datos = r
        self.pares.delete(*self.pares.get_children())
        for i, p in enumerate(r["pares"], 1):
            texto = p["par"] if p["valido"] else f'{p["par"]}  (se salta)'
            self.pares.insert("", "end", values=(i, texto), tags=() if p["valido"] else ("invalido",))
        origen = f'Secuencia {r["origen"]}' if r["origen"] else "Secuencia escrita a mano"
        aviso = "" if r["pares"] or not r["cantidadPares"] else " (lista de pares omitida: más de 200)"
        self.info.configure(text=f'{origen} · {r["cantidadPares"]} pares · {len(r["vertices"])} vértices'
                                 f' · beta = {r["beta"]:g}{aviso}. Rojo = par con letra ambigua (se salta).')
        self.app.estado("Grafo construido.")
        self.dibujar_grafo()
        self.dibujar_matriz()

    # --- Colores de la matriz: de blanco al color de acento según el peso ---
    @staticmethod
    def color_calor(t):
        a = (255, 255, 255)
        b = (14, 124, 107)
        return "#%02x%02x%02x" % tuple(int(a[i] + (b[i] - a[i]) * t) for i in range(3))

    def dibujar_matriz(self):
        c = self.lienzo_matriz
        c.delete("all")
        if not self.datos or not self.datos["vertices"]:
            return
        v, m = self.datos["vertices"], self.datos["matriz"]
        n = len(v)
        ancho, alto = c.winfo_width(), c.winfo_height()
        margen = 46
        celda = max(18, min(70, (min(ancho, alto) - margen - 30) / n))
        x0, y0 = margen + 10, margen + 10
        maximo = max(max(f) for f in m) or 1
        fuente = F["mini"] if celda < 44 else F["chica"]
        c.create_text(x0, 14, text="fila = origen (u)   ·   columna = destino (v)",
                      anchor="w", fill=TEXTO_SUAVE, font=F["chica"])
        for j, nombre in enumerate(v):
            c.create_text(x0 + j * celda + celda / 2, y0 - 12, text=nombre,
                          font=F["mono_chica_negrita"], fill=TEXTO)
        for i, nombre in enumerate(v):
            c.create_text(x0 - 10, y0 + i * celda + celda / 2, text=nombre, anchor="e",
                          font=F["mono_chica_negrita"], fill=TEXTO)
            for j in range(n):
                peso = m[i][j]
                t = (peso / maximo) ** 0.6 if peso > 0 else 0
                x, y = x0 + j * celda, y0 + i * celda
                c.create_rectangle(x, y, x + celda, y + celda, fill=self.color_calor(t), outline=BORDE)
                if celda >= 30:
                    texto = "0" if peso == 0 else (f"{peso:.3f}" if peso < 10 else f"{peso:.1f}")
                    c.create_text(x + celda / 2, y + celda / 2, text=texto, font=fuente,
                                  fill="white" if t > 0.55 else (TEXTO if peso else "#b8c1c9"))

    def dibujar_grafo(self):
        """Vértices en un círculo; el grosor de cada arco es proporcional al peso."""
        c = self.lienzo_grafo
        c.delete("all")
        if not self.datos or not self.datos["vertices"]:
            return
        v, m = self.datos["vertices"], self.datos["matriz"]
        n = len(v)
        ancho, alto = c.winfo_width(), c.winfo_height()
        cx, cy = ancho / 2, alto / 2
        radio = max(40, min(ancho, alto) / 2 - 60)
        r_nodo = 22
        pos = [(cx + radio * math.cos(2 * math.pi * i / n - math.pi / 2),
                cy + radio * math.sin(2 * math.pi * i / n - math.pi / 2)) for i in range(n)]
        maximo = max(max(f) for f in m) or 1

        # Primero los arcos (para que queden debajo de los vértices).
        for a in range(n):
            for b in range(n):
                peso = m[a][b]
                if peso <= 0:
                    continue
                t = peso / maximo
                grosor = 0.6 + 5 * t
                color = self.color_calor(0.25 + 0.75 * t)
                if a == b:
                    self.dibujar_lazo(c, pos[a], (cx, cy), r_nodo, grosor, color)
                else:
                    self.dibujar_arco(c, pos[a], pos[b], r_nodo, grosor, color)

        for i, (x, y) in enumerate(pos):
            c.create_oval(x - r_nodo, y - r_nodo, x + r_nodo, y + r_nodo,
                          fill=PANEL, outline=ACENTO, width=2)
            c.create_text(x, y, text=v[i], font=F["mono_negrita"], fill=TEXTO)
        c.create_text(12, alto - 12, anchor="sw", fill=TEXTO_SUAVE, font=F["chica"],
                      text="Grosor y color del arco = peso. Los arcos se curvan para separar u->v de v->u.")

    @staticmethod
    def dibujar_arco(c, p, q, r_nodo, grosor, color):
        (x1, y1), (x2, y2) = p, q
        dx, dy = x2 - x1, y2 - y1
        d = math.hypot(dx, dy) or 1
        ux, uy = dx / d, dy / d
        # Punto de control desplazado a la derecha de la dirección u→v.
        curva = 0.15 * d
        mx, my = (x1 + x2) / 2 - uy * curva, (y1 + y2) / 2 + ux * curva
        # Recortamos los extremos para que la flecha termine en el borde del círculo.
        def hacia(px, py, ox, oy):
            ddx, ddy = ox - px, oy - py
            dd = math.hypot(ddx, ddy) or 1
            return px + ddx / dd * r_nodo, py + ddy / dd * r_nodo
        sx, sy = hacia(x1, y1, mx, my)
        ex, ey = hacia(x2, y2, mx, my)
        c.create_line(sx, sy, mx, my, ex, ey, smooth=True, width=grosor, fill=color,
                      arrow="last", arrowshape=(8 + grosor, 10 + grosor, 3 + grosor / 2))

    @staticmethod
    def dibujar_lazo(c, p, centro, r_nodo, grosor, color):
        x, y = p
        dx, dy = x - centro[0], y - centro[1]
        d = math.hypot(dx, dy) or 1
        ux, uy = dx / d, dy / d
        lx, ly = x + ux * (r_nodo + 12), y + uy * (r_nodo + 12)
        c.create_oval(lx - 14, ly - 14, lx + 14, ly + 14, outline=color, width=grosor)


# =============================================================================
# Ventana principal
# =============================================================================
class Aplicacion(tk.Tk):
    PAGINAS = [
        ("archivo", "Archivo FASTA", "a, b", PaginaArchivo),
        ("buscar", "Buscar secuencia", "b", PaginaBuscar),
        ("kmers", "Tabla de k-mers", "b", PaginaKmers),
        ("top", "Más frecuentes", "c", PaginaTop),
        ("grafo", "Grafo de 2-mers", "d", PaginaGrafo),
    ]

    def __init__(self):
        super().__init__()
        self.title("Análisis genómico - k-mers")
        self.geometry("1180x760")
        self.minsize(960, 620)
        self.configure(bg=FONDO)
        elegir_fuentes(self)
        self.estilos()

        try:
            self.motor = MotorCpp()
        except RuntimeError as e:
            messagebox.showerror("Error", str(e))
            self.destroy()
            sys.exit(1)

        self.ids = []
        self.descripcion_kmers = ""

        # Barra lateral
        lateral = tk.Frame(self, bg=BARRA, width=230)
        lateral.pack(side="left", fill="y")
        lateral.pack_propagate(False)
        tk.Label(lateral, text="Análisis\ngenómico", bg=BARRA, fg="white", justify="left",
                 font=F["marca"]).pack(anchor="w", padx=22, pady=(24, 4))
        tk.Label(lateral, text="Motor en C++ · k-mers", bg=BARRA, fg=BARRA_TEXTO,
                 font=F["chica"]).pack(anchor="w", padx=22, pady=(0, 22))

        self.botones = {}
        for clave, texto, parte, _ in self.PAGINAS:
            b = tk.Label(lateral, text=f"  {texto}", anchor="w", bg=BARRA, fg=BARRA_TEXTO,
                         font=F["navegacion"], padx=14, pady=10, cursor="hand2")
            b.pack(fill="x", padx=10, pady=1)
            etiqueta = tk.Label(b, text=parte, bg=BARRA, fg="#7d8c99", font=F["mini"])
            etiqueta.place(relx=1, x=-10, rely=0.5, anchor="e")
            for w in (b, etiqueta):
                w.bind("<Button-1>", lambda e, c=clave: self.ir_a(c))
            self.botones[clave] = (b, etiqueta)

        self.archivo_info = tk.Label(lateral, text="Sin archivo cargado", bg=BARRA, fg=BARRA_TEXTO,
                                     font=F["chica"], justify="left", wraplength=190)
        self.archivo_info.pack(side="bottom", anchor="w", padx=22, pady=18)

        # Zona de contenido + barra de estado
        derecha = ttk.Frame(self, style="Fondo.TFrame")
        derecha.pack(side="left", fill="both", expand=True)
        self.barra_estado = tk.Label(derecha, text="Listo.", anchor="w", bg=PANEL, fg=TEXTO_SUAVE,
                                     padx=16, pady=6, font=F["chica"],
                                     highlightthickness=1, highlightbackground=BORDE)
        self.barra_estado.pack(side="bottom", fill="x")
        self.contenido = ttk.Frame(derecha, style="Fondo.TFrame")
        self.contenido.pack(fill="both", expand=True)

        self.paginas = {clave: clase(self) for clave, _, _, clase in self.PAGINAS}
        self.actual = None
        self.ir_a("archivo")
        self.protocol("WM_DELETE_WINDOW", self.salir)

        # Cargamos el archivo de ejemplo al abrir, si existe.
        if ARCHIVO_POR_DEFECTO.exists():
            self.paginas["archivo"].cargar()

    def estilos(self):
        s = ttk.Style(self)
        s.theme_use("clam")
        s.configure(".", background=FONDO, foreground=TEXTO, fieldbackground=PANEL,
                    bordercolor=BORDE, lightcolor=BORDE, darkcolor=BORDE)
        s.configure("Fondo.TFrame", background=FONDO)
        s.configure("Panel.TFrame", background=PANEL)
        s.configure("Tarjeta.TFrame", background=PANEL, relief="solid", borderwidth=1)
        s.configure("Fondo.TLabel", background=FONDO)
        s.configure("Titulo.TLabel", background=FONDO, font=F["titulo"])
        s.configure("Subtitulo.TLabel", background=FONDO, foreground=TEXTO_SUAVE)
        s.configure("Nota.TLabel", background=FONDO, foreground=TEXTO_SUAVE, font=F["chica"])
        s.configure("Resultado.TLabel", background=FONDO, foreground=ACENTO, font=F["resultado"])
        s.configure("TarjetaTitulo.TLabel", background=PANEL, foreground=TEXTO_SUAVE, font=F["chica"])
        s.configure("TarjetaValor.TLabel", background=PANEL, font=F["valor"])
        s.configure("TButton", padding=(12, 6))
        s.configure("Acento.TButton", background=ACENTO, foreground="white", bordercolor=ACENTO)
        s.map("Acento.TButton", background=[("active", "#0b6457"), ("pressed", "#094f45")])
        s.configure("Treeview", background=PANEL, fieldbackground=PANEL, rowheight=24)
        s.configure("Treeview.Heading", background="#eef1f4", font=F["cabecera"], padding=(6, 5))
        s.map("Treeview", background=[("selected", ACENTO_CLARO)], foreground=[("selected", TEXTO)])
        s.configure("TNotebook", background=FONDO, borderwidth=0)
        s.configure("TNotebook.Tab", padding=(12, 6))
        s.map("TNotebook.Tab", background=[("selected", PANEL)])

    def ir_a(self, clave, **datos):
        if self.actual:
            self.paginas[self.actual].pack_forget()
            boton, etiqueta = self.botones[self.actual]
            boton.configure(bg=BARRA, fg=BARRA_TEXTO)
            etiqueta.configure(bg=BARRA)
        self.actual = clave
        boton, etiqueta = self.botones[clave]
        boton.configure(bg=BARRA_ACTIVA, fg="white")
        etiqueta.configure(bg=BARRA_ACTIVA)
        self.paginas[clave].pack(fill="both", expand=True)
        self.paginas[clave].al_mostrar(**datos)

    def estado(self, texto, error=False):
        self.barra_estado.configure(text=texto, fg=ERROR if error else TEXTO_SUAVE)

    def archivo_cargado(self, r):
        self.ids = r["ids"]
        self.descripcion_kmers = ""
        nombre = Path(r["archivo"]).name
        self.archivo_info.configure(text=f"Archivo: {nombre}\n{r['leidas']} secuencias")
        self.estado(f"Se leyeron {r['leidas']} secuencias de {r['archivo']}.")

    def kmers_construidos(self, r):
        self.descripcion_kmers = f'Tabla actual: {r["k"]}-mers de {r["id"]} ({r["distintos"]} distintos)'
        self.estado(f'Tabla 2 creada: {r["totales"]} {r["k"]}-mers ({r["distintos"]} distintos), '
                    f'{r["colisiones"]} colisiones.')

    def salir(self):
        self.motor.cerrar()
        self.destroy()


if __name__ == "__main__":
    Aplicacion().mainloop()
