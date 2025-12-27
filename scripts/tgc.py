import cv2
import struct
import sys
import numpy as np

TGC_MAGIC = b"TGC"
TGC_VERSION = 1

TGC_RGB24  = 0
TGC_RGBA32 = 1


def make_tgc(
    input_video,
    output_tgc,
    seconds=2.0,
    fps_override=None,
    rgba=False
):
    cap = cv2.VideoCapture(input_video)
    if not cap.isOpened():
        raise RuntimeError("Failed to open video")

    src_fps = cap.get(cv2.CAP_PROP_FPS)
    fps = int(fps_override or src_fps)
    frame_count = int(seconds * fps)

    width  = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))

    pixel_format = TGC_RGBA32 if rgba else TGC_RGB24
    bpp = 4 if rgba else 3
    frame_size = width * height * bpp

    frames = []
    index  = []

    print(f"Encoding {frame_count} frames @ {fps} fps")

    for i in range(frame_count):
        ret, frame = cap.read()
        if not ret:
            break

        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

        if rgba:
            alpha = np.full((height, width, 1), 255, dtype=np.uint8)
            frame = np.concatenate((frame, alpha), axis=2)

        frames.append(frame.tobytes())

    cap.release()

    frame_count = len(frames)

    with open(output_tgc, "wb") as f:
        # ---- HEADER ----
        f.write(struct.pack(
            "<3sBHHHBBI",
            TGC_MAGIC,
            TGC_VERSION,
            width,
            height,
            fps,
            pixel_format,
            0,              # flags
            frame_count
        ))

        # ---- INDEX PLACEHOLDER ----
        index_offset = f.tell()
        f.write(b"\x00" * (frame_count * 8))

        payload_start = f.tell()

        # ---- FRAME DATA ----
        offset = 0
        for data in frames:
            f.write(data)
            index.append((offset, len(data)))
            offset += len(data)

        # ---- WRITE INDEX ----
        f.seek(index_offset)
        for off, size in index:
            f.write(struct.pack("<II", off, size))

    print(f"Saved {output_tgc}")
    print(f"{width}x{height}, frames={frame_count}, format={'RGBA' if rgba else 'RGB'}")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("usage: python make_tgc.py input.mp4 output.tgc [seconds]")
        sys.exit(1)

    seconds = float(sys.argv[3]) if len(sys.argv) > 3 else 2.0

    make_tgc(
        input_video=sys.argv[1],
        output_tgc=sys.argv[2],
        seconds=seconds,
        rgba=False
    )
