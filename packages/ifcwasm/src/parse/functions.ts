import { getModule } from "../api.js";
import * as bind from "./bindings.generated.js";
import type { FileStatus, FileType } from "./types.js";

export function version(): string {
  return bind.version(getModule());
}

export function createTimestamp(): string {
  return bind.create_timestamp(getModule());
}

export function guessFileType(path: string): FileType {
  return bind.guess_file_type(getModule(), path) as FileType;
}

export function fileStatusMessage(status: FileStatus): string {
  return bind.file_status_message(getModule(), status);
}

export function lastError(): string {
  return bind.last_error(getModule());
}
