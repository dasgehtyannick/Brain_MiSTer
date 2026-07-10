#ifndef ACHIEVEMENTS_H
#define ACHIEVEMENTS_H

#include <stdint.h>

// RetroAchievements integration for MiSTer FPGA
//
// Lifecycle:
//   1. achievements_init() — called once at startup
//   2. achievements_load_game(rom_path) — called when a ROM is loaded
//   3. achievements_poll() — called every frame from the scheduler loop
//   4. achievements_unload_game() — called when core changes or ROM unloads
//   5. achievements_deinit() — called at shutdown
//
// Debug log is written to /tmp/ra_debug.log and stdout (magenta prefix "RA:")

// Write a line to the RA log (stdout + /tmp/ra_debug.log). Safe to call from any thread.
void ra_log_write(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

// Initialize the RA subsystem. Call once after core type is known.
void achievements_init(void);

// Notify that a new game ROM was loaded. Triggers MD5 hash + game identification.
// rom_path: full path to the ROM file, crc32: CRC32 from user_io_get_file_crc()
void achievements_load_game(const char *rom_path, uint32_t crc32);

// Per-frame poll. Checks DDRAM mirror for new frame data, calls rc_client_do_frame().
void achievements_poll(void);

// Unload current game (e.g., before loading a new one or switching cores).
void achievements_unload_game(void);

// Notify RA runtime that an in-core reset happened (without unloading the game).
void achievements_notify_core_reset(void);

// Shutdown. Frees all resources.
void achievements_deinit(void);

// Returns 1 if RA is active (game loaded and mirror functional)
int achievements_active(void);

// Show RA status info popup (login, game, achievement progress).
// Safe to call anytime — silently does nothing if menu is active.
void achievements_info(void);

// Returns 1 if hardcore mode is enabled in retroachievements.cfg
int achievements_hardcore_active(void);

// Returns 1 if the user is logged in and a game is currently loaded.
int achievements_has_active_game(void);

// Open the achievement list view (builds sorted list: unlocked first, then locked).
// Returns the total number of achievements, or 0 if not available.
int achievements_list_open(void);

// Close and free the achievement list view.
void achievements_list_close(void);

// Navigate the list. Use SCANF_NEXT, SCANF_PREV, SCANF_NEXT_PAGE, etc. from file_io.h.
void achievements_list_scan(int mode);

// Render the current page of the achievement list to the OSD.
void achievements_list_print(void);

// Returns the total count in the currently open list (0 if not open).
int achievements_list_count(void);

// Switch the list view to the previous/next achievement set (dir = -1/+1)
// of a multiset game (rcheevos 12+). Returns 1 if the view changed,
// 0 for single-set games (caller should fall back to page scrolling).
int achievements_list_switch_set(int dir);

// Update global frame counters (called by per-console poll handlers)
void ra_frame_processed(uint32_t frame);
int achievements_stall_recovery_enabled(void);
int achievements_rtquery_enabled(void);
int achievements_gba_reset_ram(void);   // 1 = clear IWRAM+EWRAM on game load (retroachievements.cfg: gba_reset_ram)
int achievements_recollect_interval(void);
int achievements_smart_cache_enabled(void);
int achievements_n64_snapshot_enabled(void);

// Debug watch list from retroachievements.cfg (watch=<hex>,<hex>,...).
// Returns the count and sets *addrs to the RA-address array. Console handlers
// log every per-frame value change of these addresses.
int achievements_watch_list(const uint32_t **addrs);

// 1 = dynamic-only smart-cache prune enabled (retroachievements.cfg:
// smart_cleanup, default 1). Drops add_dynamic (AddAddress) entries ~1/min
// when they pile up; the static bootstrap set is never pruned, and pruned
// addresses that are still needed re-add themselves via rtquery misses.
int achievements_smart_cleanup_enabled(void);

// 1 = MegaDrive-only lightgun A/B test (retroachievements.cfg: justifier_test,
// default 0): keeps the smart cache but caps the rtquery busy-wait (~1ms,
// plus a 20ms fail-fast cooldown after a timeout) so no spin can delay
// input forwarding.
int achievements_justifier_test(void);

#endif // ACHIEVEMENTS_H
