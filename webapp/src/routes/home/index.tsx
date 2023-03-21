import { h } from 'preact';
import axios from 'axios';

import style from './style.css';

const Home = () => {
    const override = async (brightness: number) => {
        await axios.post('http://localhost:8000/on', { brightness });
    };

    const removeOverride = async () => {
        await axios.post('http://localhost:8000/off');
    };

    return (
        <div class={style.home}>
            <button class={style.button} onClick={() => override(0)}>Override 0%</button>
            <button class={style.button} onClick={() => override(25)}>Override 25%</button>
            <button class={style.button} onClick={() => override(50)}>Override 50%</button>
            <button class={style.button} onClick={() => override(75)}>Override 75%</button>
            <button class={style.button} onClick={() => override(100)}>Override 100%</button>
            <button class={style.button} onClick={() => removeOverride()}>Remove override</button>
        </div>
    );
};

interface ResourceProps {
    title: string;
    description: string;
    link: string;
}

const Resource = (props: ResourceProps) => {
    return (
        <a href={props.link} class={style.resource}>
            <h2>{props.title}</h2>
            <p>{props.description}</p>
        </a>
    );
};

export default Home;
