import { useState, useRef } from 'react';

function App() {
  const [codigo, setCodigo] = useState('');
  const [resultados, setResultados] = useState(null);
  const [status, setStatus] = useState('Listo para analizar');
  const fileInputRef = useRef(null);

  // Requisito: Carga de archivos .task[cite: 1, 14]
  const manejarCargaArchivo = (event) => {
    const file = event.target.files[0];
    if (!file) return;
    const reader = new FileReader();
    reader.onload = (e) => {
      setCodigo(e.target.result);
      setStatus(`Archivo ${file.name} cargado.`);
    };
    reader.readAsText(file);
  };

  const analizar = async () => {
    if (!codigo.trim()) return;
    setStatus('Analizando...');
    try {
      const response = await fetch('http://localhost:8080/analyze', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ source: codigo })
      });
      const data = await response.json();
      setResultados(data); // Aquí vienen tokens, errores y el DOT[cite: 8]
      setStatus(data.exitoso ? '¡Análisis Exitoso!' : 'Se encontraron errores.');
    } catch (err) {
      setStatus('Error: No se pudo conectar al backend.');
    }
  };

  return (
    <div style={{ backgroundColor: '#0f172a', color: '#f8fafc', minHeight: '100vh', padding: '30px', fontFamily: 'Inter, sans-serif' }}>
      <header style={{ marginBottom: '20px', borderBottom: '1px solid #334155', paddingBottom: '10px' }}>
        <h1 style={{ margin: 0, color: '#38bdf8' }}>TaskScript IDE v2.0</h1>
        <p style={{ color: '#94a3b8' }}>{status}</p>
      </header>

      <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', gap: '20px' }}>
        
        {/* LADO IZQUIERDO: EDITOR Y CARGA[cite: 1, 14] */}
        <div>
          <div style={{ marginBottom: '10px', display: 'flex', gap: '10px' }}>
            <input type="file" accept=".task" ref={fileInputRef} onChange={manejarCargaArchivo} style={{ display: 'none' }} />
            <button onClick={() => fileInputRef.current.click()} style={btnStyle('#334155')}>Cargar .task</button>
            <button onClick={analizar} style={btnStyle('#0284c7')}>▶ Ejecutar Análisis</button>
          </div>
          <textarea 
            value={codigo}
            onChange={(e) => setCodigo(e.target.value)}
            style={{ width: '100%', height: '500px', backgroundColor: '#1e293b', color: '#e2e8f0', padding: '15px', borderRadius: '8px', border: '1px solid #334155', fontFamily: 'Fira Code, monospace', fontSize: '14px' }}
            placeholder='TABLERO "Proyecto" { ... }'
          />
          
          <div style={{ marginTop: '15px', display: 'grid', gridTemplateColumns: '1fr 1fr', gap: '10px' }}>
            <button onClick={() => window.open('http://localhost:8080/reporte_kanban')} style={btnStyle('#7c3aed')}>Tablero Kanban (HTML)</button>
            <button onClick={() => window.open('http://localhost:8080/reporte_responsables')} style={btnStyle('#db2777')}>Carga Responsables (HTML)</button>
            <button onClick={() => window.open('http://localhost:8080/reporte_tokens')} style={btnStyle('#059669')}>Tabla Tokens (HTML)</button>
            <button onClick={() => window.open('http://localhost:8080/reporte_errores')} style={btnStyle('#dc2626')}>Tabla Errores (HTML)</button>
          </div>
        </div>

        {/* LADO DERECHO: CONSOLA DE RESULTADOS[cite: 1, 14] */}
        <div style={{ display: 'flex', flexDirection: 'column', gap: '20px' }}>
          
          {/* TABLA DE ERRORES */}
          <div style={panelStyle('#450a0a')}>
            <h4 style={{ margin: '0 0 10px 0' }}>Errores Sintácticos/Léxicos</h4>
            <div style={{ overflowY: 'auto', maxHeight: '150px' }}>
              {resultados?.errores?.length > 0 ? (
                <table style={tableStyle}>
                  <thead><tr><th>Token</th><th>Tipo</th><th>Línea</th><th>Descripción</th></tr></thead>
                  <tbody>
                    {resultados.errores.map((e, i) => (
                      <tr key={i}><td>{e.lexema}</td><td>{e.tipo}</td><td>{e.linea}</td><td>{e.descripcion}</td></tr>
                    ))}
                  </tbody>
                </table>
              ) : <p style={{ fontSize: '12px' }}>Sin errores detectados.</p>}
            </div>
          </div>

          {/* TABLA DE TOKENS[cite: 1, 8] */}
          <div style={panelStyle('#064e3b')}>
            <h4 style={{ margin: '0 0 10px 0' }}>Tokens Encontrados</h4>
            <div style={{ overflowY: 'auto', maxHeight: '150px' }}>
              <table style={tableStyle}>
                <thead><tr><th>Lexema</th><th>Tipo</th><th>Línea</th><th>Columna</th></tr></thead>
                <tbody>
                  {resultados?.tokens?.map((t, i) => (
                    <tr key={i}><td>{t.lexema}</td><td>{t.tipo}</td><td>{t.linea}</td><td>{t.columna}</td></tr>
                  ))}
                </tbody>
              </table>
            </div>
          </div>

          {/* CÓDIGO GRAPHVIZ DOT[cite: 1, 8] */}
          <div style={panelStyle('#1e293b')}>
            <h4 style={{ margin: '0 0 10px 0' }}>Árbol de Derivación (DOT)</h4>
            <textarea 
              readOnly 
              value={resultados?.dot || ""} 
              style={{ width: '100%', height: '100px', backgroundColor: '#0f172a', color: '#94a3b8', border: 'none', fontSize: '11px' }} 
            />
          </div>
        </div>

      </div>
    </div>
  );
}

// Estilos auxiliares
const btnStyle = (bg) => ({ backgroundColor: bg, color: 'white', border: 'none', padding: '10px 15px', borderRadius: '6px', fontWeight: 'bold', cursor: 'pointer', fontSize: '13px' });
const panelStyle = (bg) => ({ backgroundColor: bg, padding: '15px', borderRadius: '8px', border: '1px solid #334155' });
const tableStyle = { width: '100%', borderCollapse: 'collapse', fontSize: '12px', textAlign: 'left' };

export default App;