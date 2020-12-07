target_sources(utility PUBLIC circular_buffer.c utility_math.c pointer_utils.c timing.c)
TARGET_SOURCES(circular_buffer_test PUBLIC
  circular_buffer.c circular_buffer_test.c
  )
TARGET_SOURCES(timing_test PUBLIC timing_test.c)
