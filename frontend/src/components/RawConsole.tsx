interface RawConsoleProps {
  lines: string[];
}

export function RawConsole({ lines }: RawConsoleProps) {
  return (
    <section className="panel">
      <div className="panel__header">
        <div>
          <p className="eyebrow">Serial Feed</p>
          <h2>Raw Incoming NDJSON</h2>
        </div>
      </div>
      <div className="console">
        {lines.length === 0 ? <p>No data received yet.</p> : null}
        {[...lines].reverse().map((line, index) => (
          <code key={`${index}-${line.slice(0, 24)}`}>{line}</code>
        ))}
      </div>
    </section>
  );
}

