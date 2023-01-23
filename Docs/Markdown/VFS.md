# Virtual File System {#VFS}

## Paths
| Path | Description |
| ---: | :---------- |
| `app://` | Directory of launched executable |
| `data://` | Persistent data directory. <br>`%APPDATA%/Aqua Engine` on Windows<br>`~/Library/Caches/Aqua Engine/` on Mac OS<br>`./.data/` on Linux |
| `file://` | Used to access files on the filesystem using absolute paths |
| `assets://` | `Assets` directory located in same directory as executable |

### Editor-Specific Paths
| Path | Description |
| ---: | :---------- |
| `project://` | Root directory of project. This is where `project.json` is located |
| `editor://` | Editor cache of open project. Located at equivalent of `project://.aqua/` |
| `assets://` | In addition to the regular assets, the default assets bundled with the editor are also mounted here. This folder is located at `app://Assets` (*while editor is open*) |