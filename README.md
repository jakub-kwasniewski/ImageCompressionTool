
# Image Compression Tool

## Project Description

The project involves creating a specification for a raster graphic file format and an application that allows converting a .bmp file to a custom graphic format .z25, and vice versa.

The application allows customization of the conversion:

**Choice of write mode:**
- RGB565
- RGB888
- YCbCr888

**Choice of color mode:**
- Color
- Grayscale

**Choice of dithering:**
- No dithering
- Bayer dithering

**Choice of compression type:**
- Lossless (LZW)
- Lossy (DCT transform)
- None

**Choice of prediction algorithm:**
- None
- Type 1 prediction

**Choice of chrominance component subsampling? (Only for YCbCr)**
- No
- Yes

The application allows for any configuration of the described modes, except for lossy compression, where the prediction algorithm is not available, and chrominance subsampling, which is only available in YCbCr write mode.

The header of the .z25 file is used to identify the file and read the metadata related to the image. The elements of the header and their meaning are shown in the table below:

| **Name**                 | **Offset** | **Size** | **Values and Purpose**                                                                           |
| ------------------------ | ---------- | -------- | ------------------------------------------------------------------------------------------------ |
| **Identifier**           | 00         | 2 bytes  | "KW" – used to identify the file                                                                 |
| **Image width**          | 02         | 2 bytes  | Specifies the width of the image                                                                 |
| **Image height**         | 04         | 2 bytes  | Specifies the height of the image                                                                |
| **Write mode**           | 06         | 1 byte   | Values: 1-3  <br>  <br>1 – RGB565  <br>2 – RGB888  <br>3 – YCbCr888                              |
| **Color mode**           | 07         | 1 byte   | Values: 0-1  <br>  <br>0 – Color  <br>1 – Grayscale                                              |
| **Dithering**            | 08         | 1 byte   | Values: 0-1  <br>  <br>0 – No dithering  <br>1 – Bayer dithering                                 |
| **Prediction algorithm** | 09         | 1 byte   | Values: 0-1  <br>  <br>0 – None  <br>1 – Type 1 prediction                                       |
| **Compression type**     | 10         | 1 byte   | Values: 0-2  <br><br>0 – Lossless compression  <br>1 – Lossy compression  <br>2 – No compression |

## Conversion description

#### I. Model and Write Mode

- **RGB565**  
    Data is stored as 16 bits, where the first 5 bits represent the red color, the next 6 bits green color, and the last 5 bits the blue color.  
    In the case of grayscale, the data is stored as a full 16-bit value (range 0 – 65535), which helps with calculation accuracy but may lead to rounding errors.
    
- **RGB888**  
    The red data is written to the file first, followed by the green data, and finally the blue data.  
    Grayscale is stored using only 8 bits.
    
- **YCbCr888**  
    The data is stored similarly to RGB888: first all Y values, then all Cb values, and finally all Cr values.  
    For grayscale, the Y value contains the grayscale information, so only this value is stored.
    

Data is read in the same order and, if necessary, converted to specific RGB values.  
After reading a grayscale image, the same value is assigned to all RGB channels.

---

#### II. Bayer Dithering Method

All RGB components are individually subjected to dithering. In the case of YCbCr, dithering is performed before the conversion to this model.

---

#### III. Compression Type

- **Lossless LZW Compression**  
    All components are compressed one by one and stored as 8-bit values.
    
- **DCT Transformation**  
    Each component undergoes a transformation separately and is written to the file sequentially. In the case of RGB565, the values are stored as 16-bit values.
    

No compression (as described in Section I) is used in the case where no compression is applied.

---

#### IV. Prediction Algorithm

Type 1 prediction is used, meaning that each value in a row is the result of: current value – previous value.  
Each component undergoes this process separately. The values are stored sequentially, with RGB565 stored as 16-bit values.

---

#### V. Component Subsampling

This operation is only performed if the YCbCr888 model is selected. 4:2:0 subsampling mode was used, which means that chrominance values are averaged in a 2x2 block.
