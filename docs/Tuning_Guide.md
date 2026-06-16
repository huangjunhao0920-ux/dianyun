# Tuning Guide

## Real-time Targets
- Frame rate: >= 30 FPS
- End-to-end latency: < 33 ms per processing cycle

## Recommended Initial Parameters
- LiDAR input rate: 10-20 Hz (single-line)
- IMU input rate: 100-400 Hz
- Stitching window size: 0.5-1.5 s
- Ground threshold (z): 0.1-0.2 m

## Latency Budget Example (33 ms)
- Sensor ingest + sync: 5 ms
- IMU pose update + stitching: 10 ms
- Classification + coloring: 8 ms
- Rendering + display transfer: 10 ms

## Optimization Checklist
1. Use lock-free queues or bounded queues for high-rate IMU path.
2. Keep point copy counts low; prefer move semantics.
3. Batch color projection and use image pyramid for camera sampling.
4. Use spatial downsampling before visualization when cloud density spikes.
5. Pin critical threads and prioritize sync/processing threads in MDC runtime.
