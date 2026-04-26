import sys
import os
from PIL import Image, ImageOps

def convert_to_rgb565(input_path, width, height, mode="estirar"):
    # Configuración de salida
    base_name = os.path.splitext(input_path)[0]
    output_h_path = f"{base_name}.h"
    array_name = os.path.basename(base_name).replace("-", "_").replace(" ", "_")
    
    # Tamaño objetivo pasado por el usuario
    target_size = (width, height)

    try:
        img_orig = Image.open(input_path).convert("RGBA")
        
        # 1. Aplicar el modo de ajuste seleccionado
        if mode == "recortar":
            # Recorta la imagen para llenar el área sin deformar
            img_proc = ImageOps.fit(img_orig, target_size, method=Image.Resampling.LANCZOS)
        
        elif mode == "barras":
            # Escala manteniendo proporción y añade fondo (Magenta para transparencia)
            img_orig.thumbnail(target_size, Image.Resampling.LANCZOS)
            # Usamos Magenta (255, 0, 255) como fondo por defecto para tus sprites
            img_proc = Image.new("RGBA", target_size, (255, 0, 255, 255))
            x = (target_size[0] - img_orig.width) // 2
            y = (target_size[1] - img_orig.height) // 2
            img_proc.paste(img_orig, (x, y), img_orig)
            
        else: # modo "estirar"
            img_proc = img_orig.resize(target_size, Image.Resampling.LANCZOS)

        # 2. Aplanar imagen (El color de fondo aquí es Magenta si hay transparencias)
        final_img = Image.new("RGB", target_size, (255, 0, 255))
        final_img.paste(img_proc, (0, 0), img_proc if img_proc.mode == "RGBA" else None)
        
        pixels = list(final_img.getdata())
        
    except Exception as e:
        print(f"Error al procesar la imagen: {e}")
        sys.exit(1)

    # 3. Generar el archivo .h
    with open(output_h_path, 'w') as f:
        f.write("#include <stdint.h>\n\n")
        f.write(f"// Imagen: {input_path} | Dimensiones: {width}x{height} | Modo: {mode}\n")
        f.write(f"const uint16_t {array_name}_map[] = {{\n")
        
        for i, (r, g, b) in enumerate(pixels):
            # Conversión a RGB565
            rgb = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
            # Swap de bytes para que el ILI9341 lo lea correctamente vía SPI
            rgb_swapped = ((rgb & 0xFF) << 8) | ((rgb >> 8) & 0xFF)
            
            f.write(f"0x{rgb_swapped:04X}, ")
            if (i + 1) % 12 == 0: f.write("\n")
                
        f.write("\n};")
    
    print(f"Éxito: {output_h_path} generado ({width}x{height}) en modo '{mode}'")

if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Uso: python3 script.py <imagen> <ancho> <alto> [estirar | recortar | barras]")
        print("Ejemplo: python3 script.py ojo.png 20 20 barras")
    else:
        img_path = sys.argv[1]
        try:
            w = int(sys.argv[2])
            h = int(sys.argv[3])
        except ValueError:
            print("Error: El ancho y el alto deben ser números enteros.")
            sys.exit(1)
            
        selected_mode = sys.argv[4] if len(sys.argv) > 4 else "estirar"
        convert_to_rgb565(img_path, w, h, selected_mode)