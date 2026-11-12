interface StatusBadgeProps {
  label: string;
  tone: "ok" | "warn" | "danger" | "neutral";
}

export function StatusBadge({ label, tone }: StatusBadgeProps) {
  return <span className={`status-badge status-badge--${tone}`}>{label}</span>;
}

